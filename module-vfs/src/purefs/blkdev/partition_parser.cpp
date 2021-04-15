// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/partition_parser.hpp>
#include <purefs/blkdev/disk.hpp>
#include <log/log.hpp>
#include <errno.h>
#include <limits>

namespace purefs::blkdev::internal
{

    namespace defs
    {
        namespace
        {
            constexpr auto mbr_signature_offs = 0x1FE;
            constexpr auto mbr_ptbl_offs      = 0x1BE;
            constexpr auto mbr_ptbl_active    = 0x000;
            constexpr auto mbr_ptbl_type      = 0x004;
            constexpr auto mbr_ptbl_sect_cnt  = 0x00c;
            constexpr auto mbr_ptbl_lba       = 0x008;
            constexpr auto ptbl_offs          = 0x1be;
            constexpr auto ptbl_size          = 16;
            constexpr auto ext_part           = 0x05;
            constexpr auto ext_linux_part     = 0x85;
            constexpr auto ext_win98_part     = 0x0f;
            constexpr auto reserved_sect      = 0x00e;
            constexpr auto number_of_fats     = 0x010;
            constexpr auto num_parts          = 4;
            constexpr auto min_sector_size    = 512;
        } // namespace
    }     // namespace defs
    namespace
    {
        inline auto to_word(const std::vector<uint8_t> &vec, std::size_t offs)
        {
            auto buf = &vec[offs];
            return (uint32_t(buf[0]) << 0U) | (uint32_t(buf[1]) << 8U) | (uint32_t(buf[2]) << 16U) |
                   (uint32_t(buf[3]) << 24U);
        }
        inline auto to_short(const std::vector<uint8_t> &vec, std::size_t offs)
        {
            auto buf = &vec[offs];
            return (uint16_t(buf[0]) << 0U) | (uint16_t(buf[1]) << 8U);
        }
    } // namespace

    // Partition search
    auto partition_parser::partition_search() -> int
    {
        const auto sect_size = m_disk->get_info(info_type::sector_size, 0);
        if (sect_size < 0) {
            LOG_ERROR("Unable to get sector size");
            return sect_size;
        }
        std::vector<std::uint8_t> mbr_sect(sect_size);
        auto ret = m_disk->read(mbr_sect.data(), 0, 1, 0);
        if (ret < 0) {
            return ret;
        }
        if (sect_size < defs::min_sector_size) {
            LOG_ERROR("Unable to scan partition when sector size < 512");
            return -ENXIO;
        }
        // Check initial signature
        if ((mbr_sect[defs::mbr_signature_offs] != 0x55) && (mbr_sect[defs::mbr_signature_offs + 1] != 0xAA)) {
            LOG_ERROR("Unable to find valid partition signature");
            return -ENXIO;
        }
        /* Copy the 4 partition records into partitions */
        std::array<partition, defs::num_parts> root_part;
        read_partitions(mbr_sect, root_part);
        // Add not extended partitions
        int part_no{1};
        for (auto &part : root_part) {
            if (is_extended(part.type))
                continue;

            if (!check_partition(m_disk, part))
                continue;

            if (part.num_sectors) {
                part.physical_number = part_no;
                part.mbr_number      = part_no;
                m_parts.emplace_back(part);
            }
            ++part_no;
        }
        for (const auto &part : root_part) {
            if (is_extended(part.type)) {
                ret = parse_extended(part.start_sector, part.num_sectors);
                if (ret < 0)
                    return ret;
            }
        }
        return ret;
    }

    auto partition_parser::check_partition(const std::shared_ptr<disk> disk, const partition &part) -> bool
    {
        auto sector_size     = disk->get_info(info_type::sector_size, 0);
        const auto this_size = uint64_t(disk->get_info(info_type::sector_count, 0)) * uint64_t(sector_size);
        const auto poffset   = uint64_t(part.start_sector) * uint64_t(sector_size);
        const auto psize     = uint64_t(part.num_sectors) * uint64_t(sector_size);
        const auto pnext     = uint64_t(part.start_sector) * uint64_t(sector_size) + poffset;
        if ((poffset + psize > this_size) ||                    // oversized
            (pnext < uint64_t(part.start_sector) * sector_size) // going backward
        ) {
            LOG_WARN("Part %d looks strange: start_sector %u offset %u next %u\n",
                     unsigned(part.mbr_number),
                     unsigned(part.start_sector),
                     unsigned(poffset),
                     unsigned(pnext));
            return false;
        }
        return true;
    }

    auto partition_parser::read_partitions(const std::vector<uint8_t> &buffer,
                                           std::array<partition, defs::num_parts> &parts) -> void
    {
        std::size_t offs = defs::ptbl_offs;
        for (auto &part : parts) {
            part.bootable     = buffer[defs::mbr_ptbl_active + offs] & 0x80;
            part.boot_unit    = buffer[defs::mbr_ptbl_active + offs] & 0x7F;
            part.type         = buffer[defs::mbr_ptbl_type + offs];
            part.num_sectors  = to_word(buffer, defs::mbr_ptbl_sect_cnt + offs);
            part.start_sector = to_word(buffer, defs::mbr_ptbl_lba + offs);
            offs += defs::ptbl_size;
        }
    }
    auto partition_parser::is_extended(uint8_t type) -> bool
    {
        return type == defs::ext_linux_part || type == defs::ext_part || type == defs::ext_win98_part;
    }

    auto partition_parser::parse_extended(uint32_t lba, uint32_t count) -> int
    {
        static constexpr auto max_parts{100};
        auto sector_size = m_disk->get_info(info_type::sector_size, 0);
        int extended_part_num;
        std::array<partition, defs::num_parts> parts;
        auto current_sector     = lba;
        unsigned long this_size = count * sector_size;
        if (sector_size < 0) {
            return sector_size;
        }
        uint32_t sector_in_buf{std::numeric_limits<uint32_t>::max()};
        std::vector<uint8_t> sect_buf(sector_size);

        auto try_count{max_parts};
        int error{};
        while (try_count--) {
            if (sector_in_buf != current_sector) {
                LOG_INFO("extended parse: Read sector %u\n", unsigned(current_sector));
                error = m_disk->read(sect_buf.data(), current_sector, 1, 0);
                if (error < 0)
                    break;
                sector_in_buf = current_sector;
            }
            {
                const auto b1 = sect_buf[defs::mbr_signature_offs];
                const auto b2 = sect_buf[defs::mbr_signature_offs + 1];
                if (b1 != 0x55 || b2 != 0xAA) {
                    LOG_ERROR("extended_parse: No signature %02x,%02x", b1, b2);
                    break;
                }
            }

            read_partitions(sect_buf, parts);
            extended_part_num = -1;

            for (auto partition_num = 0U; partition_num < parts.size(); ++partition_num) {
                if (parts[partition_num].num_sectors == 0) {
                    /* Partition is empty */
                    continue;
                }
                if (is_extended(parts[partition_num].type)) {
                    if (extended_part_num < 0)
                        extended_part_num = partition_num;
                    continue; /* We'll examine this ext partition later */
                }

                /* Some sanity checks */
                const auto poffset = parts[partition_num].start_sector * sector_size;
                const auto psize   = parts[partition_num].num_sectors * sector_size;
                const auto pnext   = current_sector * sector_size + poffset;
                if ((poffset + psize > this_size) ||                                  // oversized
                    (pnext < static_cast<unsigned long>(lba * sector_size)) ||        // going backward
                    (pnext > static_cast<unsigned long>((lba + count) * sector_size)) // outsized
                ) {
                    LOG_WARN("Part %d looks strange: current_sector %u offset %u next %u\n",
                             int(partition_num),
                             unsigned(current_sector),
                             unsigned(poffset),
                             unsigned(pnext));
                    continue;
                }
                parts[partition_num].physical_number = partition_num + 1;
                m_parts.emplace_back(parts[partition_num]);
                m_parts.back().start_sector += current_sector;
                try_count = max_parts;
            }

            if (extended_part_num < 0) {
                LOG_DEBUG("No more extended partitions");
                break; /* nothing left to do */
            }
            /* Examine the next extended partition */
            current_sector = lba + parts[extended_part_num].start_sector;
            this_size      = parts[extended_part_num].num_sectors * sector_size;
        }
        return error;
    }
} // namespace purefs::blkdev::internal
