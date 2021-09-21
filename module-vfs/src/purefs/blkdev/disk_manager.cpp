// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk.hpp>
#include <log/log.hpp>
#include <mutex.hpp>
#include <errno.h>
#include <charconv>
#include <tuple>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/partition_parser.hpp>

/** NOTE: Device manager implementation without sector cache
 */
namespace purefs::blkdev
{
    namespace
    {
        using namespace std::literals;
        static constexpr auto part_suffix    = "part"sv;
        static constexpr auto syspart_suffix = "sys"sv;
    } // namespace

    disk_manager::disk_manager() : m_lock(std::make_unique<cpp_freertos::MutexRecursive>())
    {}

    disk_manager::~disk_manager()
    {}

    auto disk_manager::register_device(std::shared_ptr<disk> disk, std::string_view device_name, unsigned flags) -> int
    {
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -EINVAL;
        }
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto ret = m_dev_map.find(std::string(device_name));
        if (ret != std::end(m_dev_map)) {
            LOG_ERROR("Disc with same name already registered");
            return -EEXIST;
        }
        else {
            auto ret = disk->probe(flags);
            if (ret < 0) {
                LOG_ERROR("Unable to probe the disc errno %i", ret);
                return ret;
            }
            const auto it = m_dev_map.emplace(std::make_pair(device_name, disk));
            if (flags & flags::no_parts_scan) {
                disk->clear_partitions();
                return 0;
            }
            else {
                ret = reread_partitions(std::make_shared<internal::disk_handle>(disk, it.first->first));
                return ret;
            }
        }
    }
    auto disk_manager::unregister_device(std::string_view device_name) -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        auto it = m_dev_map.find(std::string(device_name));
        if (it == std::end(m_dev_map)) {
            LOG_ERROR("Disc with given name doesn't exists in manager");
            return -ENOENT;
        }
        auto ret = it->second->cleanup();
        m_dev_map.erase(it);
        if (ret < 0) {
            LOG_ERROR("Disk cleanup failed code %i", ret);
        }
        return ret;
    }

    auto disk_manager::device_handle(std::string_view device_name) const -> disk_fd
    {
        const auto [dev, part] = parse_device_name(device_name);
        disk_fd ret{};
        if (dev.empty()) {
            ret = nullptr;
        }
        else {
            cpp_freertos::LockGuard _lck(*m_lock);
            const auto it = m_dev_map.find(std::string(dev));
            if (it == std::end(m_dev_map)) {
                ret = nullptr;
            }
            else {
                ret = std::make_shared<internal::disk_handle>(it->second, device_name, part);
                if (internal::disk_handle::is_user_partition(part)) {
                    if (part >= int(partitions(ret).size())) {
                        LOG_ERROR("Partition %i doesn't exists", part);
                        ret = nullptr;
                    }
                }
            }
        }
        return ret;
    }

    auto disk_manager::parse_device_name(std::string_view device) -> std::tuple<std::string_view, part_t>
    {
        const auto parti    = device.rfind(part_suffix);
        const auto sysparti = device.rfind(syspart_suffix);
        if ((parti != std::string::npos) != (sysparti != std::string::npos)) {
            const auto si        = (parti != std::string::npos) ? (parti) : (sysparti);
            const auto sl        = (parti != std::string::npos) ? (part_suffix.length()) : (syspart_suffix.length());
            const auto part_name = device.substr(0, si);
            const auto part_num  = device.substr(si + sl);
            part_t part_inum{-1};
            if (!part_num.empty()) {
                const auto ires = std::from_chars(std::begin(part_num), std::end(part_num), part_inum);
                if (ires.ec == std::errc()) {
                    if (sysparti != std::string::npos)
                        part_inum = internal::disk_handle::to_syspart_num(part_inum);
                    return std::make_tuple(part_name, part_inum);
                }
                else {
                    return std::make_tuple(""sv, -1);
                }
            }
            else {
                return std::make_tuple(part_name, part_inum);
            }
        }
        else {
            return std::make_tuple(device, -1);
        }
    }
    auto disk_manager::part_lba_to_disk_lba(disk_fd disk, sector_t part_lba, size_t count) -> scount_t
    {
        if (disk->is_user_partition()) {
            auto pdisc = disk->disk();
            if (!pdisc) {
                LOG_ERROR("Unable to lock disk");
                return -EIO;
            }
            const auto part = pdisc->partitions()[disk->partition()];
            if (part_lba + count > part.num_sectors) {
                LOG_ERROR("Partition sector req out of range");
                return -ERANGE;
            }
            else {
                return part_lba + part.start_sector;
            }
        }
        else {
            if (part_lba + count > disk->sectors()) {
                LOG_ERROR("Disk sector req out of range");
                return -ERANGE;
            }
            else {
                return part_lba;
            }
        }
    }
    auto disk_manager::write(disk_fd dfd, const void *buf, sector_t lba, std::size_t count) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->write(buf, calc_lba, count, dfd->system_partition());
        }
    }
    auto disk_manager::read(disk_fd dfd, void *buf, sector_t lba, std::size_t count) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->read(buf, calc_lba, count, dfd->system_partition());
        }
    }
    auto disk_manager::erase(disk_fd dfd, sector_t lba, std::size_t count) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->erase(calc_lba, count, dfd->system_partition());
        }
    }
    auto disk_manager::sync(disk_fd dfd) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->sync();
    }
    auto disk_manager::pm_control(disk_fd dfd, pm_state target_state) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->pm_control(target_state);
    }
    auto disk_manager::pm_control(pm_state target_state) -> int
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        int last_err{};
        for (const auto &disk : m_dev_map) {
            auto err = disk.second->pm_control(target_state);
            if (err) {
                LOG_ERROR("Unable to change PM state for specified device. Errno: %i", err);
                last_err = err;
            }
        }
        return last_err;
    }
    auto disk_manager::pm_read(disk_fd dfd, pm_state &current_state) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->pm_read(current_state);
    }
    auto disk_manager::status(disk_fd dfd) const -> media_status
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return media_status::error;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return media_status::error;
        }
        return disk->status();
    }
    auto disk_manager::partitions(disk_fd dfd) const -> std::vector<partition>
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return {};
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        return disk->partitions();
    }
    auto disk_manager::partition_info(disk_fd dfd) const -> std::optional<partition>
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return std::nullopt;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return std::nullopt;
        }
        if (dfd->is_user_partition()) {
            auto parts = disk->partitions();
            if (size_t(dfd->partition()) >= parts.size()) {
                LOG_ERROR("Partition num out of range");
                return std::nullopt;
            }
            else {
                return {parts[dfd->partition()]};
            }
        }
        else {
            LOG_ERROR("No paritions on disc");
            return std::nullopt;
        }
    }
    auto disk_manager::get_info(disk_fd dfd, info_type what) const -> scount_t
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return {};
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        //! When it is partition as for partition sectors count
        if (what == info_type::sector_count && dfd->is_user_partition()) {
            if (unsigned(dfd->partition()) >= disk->partitions().size()) {
                LOG_ERROR("Partition number out of range");
                return -ERANGE;
            }
            const auto part = disk->partitions()[dfd->partition()];
            return part.num_sectors;
        }
        else {
            return disk->get_info(what, dfd->system_partition());
        }
    }
    auto disk_manager::reread_partitions(disk_fd dfd) -> int
    {
        if (!dfd) {
            LOG_ERROR("Disk handle doesn't exists");
            return -EINVAL;
        }
        auto disk = dfd->disk();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        disk->clear_partitions();
        internal::partition_parser pparser(disk, disk->partitions());
        auto ret = pparser.partition_search();
        // Fill the partition name
        if (!ret) {
            int no{};
            for (auto &parts : disk->partitions()) {
                parts.name = dfd->name();
                parts.name += part_suffix;
                parts.name += std::to_string(no++);
            }
        }
        return ret;
    }

    auto disk_manager::write(std::string_view device_name, const void *buf, sector_t lba, std::size_t count) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return write(dfd, buf, lba, count);
        else
            return -ENOENT;
    }
    auto disk_manager::read(std::string_view device_name, void *buf, sector_t lba, std::size_t count) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return read(dfd, buf, lba, count);
        else
            return -ENOENT;
    }
    auto disk_manager::erase(std::string_view device_name, sector_t lba, std::size_t count) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return erase(dfd, lba, count);
        else
            return -ENOENT;
    }
    auto disk_manager::sync(std::string_view device_name) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return sync(dfd);
        else
            return -ENOENT;
    }
    auto disk_manager::pm_control(std::string_view device_name, pm_state target_state) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return pm_control(dfd, target_state);
        else
            return -ENOENT;
    }
    auto disk_manager::pm_read(std::string_view device_name, pm_state &current_state) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return pm_read(dfd, current_state);
        else
            return -ENOENT;
    }
    auto disk_manager::status(std::string_view device_name) const -> media_status
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return status(dfd);
        else
            return media_status::error;
    }
    auto disk_manager::partitions(std::string_view device_name) const -> std::vector<partition>
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return partitions(dfd);
        else
            return {};
    }
    auto disk_manager::partition_info(std::string_view device_name) const -> std::optional<partition>
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return partition_info(dfd);
        else
            return std::nullopt;
    }
    auto disk_manager::get_info(std::string_view device_name, info_type what) const -> scount_t
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return get_info(dfd, what);
        else
            return -ENOENT;
    }
    auto disk_manager::reread_partitions(std::string_view device_name) -> int
    {
        auto dfd = device_handle(device_name);
        if (dfd)
            return reread_partitions(dfd);
        else
            return -ENOENT;
    }
    auto disk_manager::disk_handle_from_partition_handle(disk_fd disk) -> disk_fd
    {
        const auto new_name = std::get<0>(parse_device_name(disk->name()));
        return std::make_shared<internal::disk_handle>(disk->disk(), new_name);
    }

} // namespace purefs::blkdev
