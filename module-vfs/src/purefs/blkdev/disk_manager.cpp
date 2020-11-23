// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk.hpp>
#include <log/log.hpp>
#include <errno.h>
#include <charconv>
#include <tuple>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/partition_parser.hpp>

/** NOTE: Device manager implementation without sector cache
 */
namespace purefs::blkdev
{
    auto disk_manager::register_device(std::shared_ptr<disk> disk, std::string_view device_name, unsigned flags) -> int
    {
        {
            cpp_freertos::LockGuard _lck(m_lock);
            const auto ret = m_dev_map.find(std::string(device_name));
            if (ret != std::end(m_dev_map)) {
                LOG_ERROR("Disc: %.*s already registered.", int(device_name.length()), device_name.data());
                return -EEXIST;
            }
            else {
                auto ret = disk->probe(flags);
                if (ret < 0) {
                    LOG_ERROR("Unable to probe the disc errno %i", ret);
                    return ret;
                }
                ret = disk->get_info(info_type::sector_count);
                if (ret < 0) {
                    LOG_ERROR("Unable to probe the disc errno %i", ret);
                    return ret;
                }
                m_dev_map.emplace(std::make_pair(device_name, disk));
                return reread_partitions(std::make_shared<internal::disk_handle>(disk, ret));
            }
        }
    }
    auto disk_manager::unregister_device(std::string_view device_name) -> int
    {
        cpp_freertos::LockGuard _lck(m_lock);
        auto it = m_dev_map.find(std::string(device_name));
        if (it == std::end(m_dev_map)) {
            LOG_ERROR("Disc: %.*s doesn't exists in manager.", int(device_name.length()), device_name.data());
            return -ENOENT;
        }
        m_dev_map.erase(it);
        auto ret = it->second->cleanup();
        if (ret < 0) {
            LOG_ERROR("Disk cleanup failed code %i", ret);
        }
        return ret;
    }

    auto disk_manager::device_handle(std::string_view device_name) const -> disk_fd_t
    {
        const auto [dev, part] = parse_device_name(device_name);
        disk_fd_t ret{};
        if (dev.empty()) {
            ret = nullptr;
        }
        else {
            cpp_freertos::LockGuard _lck(m_lock);
            const auto it = m_dev_map.find(std::string(dev));
            if (it == std::end(m_dev_map)) {
                ret = nullptr;
            }
            else {
                ret = std::make_shared<internal::disk_handle>(it->second, part);
            }
        }
        return ret;
    }

    auto disk_manager::parse_device_name(std::string_view device) -> std::tuple<std::string_view, short>
    {
        using namespace std::literals;
        static constexpr auto part_suffix = "part"sv;
        auto ret                          = device.rfind(part_suffix);
        if (ret != std::string::npos) {
            auto part_name = device.substr(0, ret);
            auto part_num  = device.substr(ret + part_suffix.length());
            short part_inum{-1};
            if (!part_num.empty()) {
                auto ires = std::from_chars(std::begin(part_num), std::end(part_num), part_inum);
                if (ires.ec != std::errc())
                    return std::make_tuple(part_name, part_inum);
                else
                    return std::make_tuple(""sv, -1);
            }
            else {
                return std::make_tuple(part_name, part_inum);
            }
        }
        else {
            return std::make_tuple(""sv, -1);
        }
    }
    auto disk_manager::part_lba_to_disk_lba(disk_fd_t disk, sector_t part_lba, size_t count) -> scount_t
    {
        if (!disk->has_partition()) {
            if (part_lba + count > disk->sectors()) {
                LOG_ERROR("Disk sector req out of range");
                return -ERANGE;
            }
            else {
                return part_lba;
            }
        }
        else {
            auto pdisc      = disk->disk().lock();
            const auto part = pdisc->partitions()[disk->partition()];
            if (part_lba + count > part.num_sectors) {
                LOG_ERROR("Partition sector req out of range");
                return -ERANGE;
            }
            else {
                return part_lba + part.start_sector;
            }
        }
    }
    auto disk_manager::write(disk_fd_t dfd, const void *buf, sector_t lba, std::size_t count) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->write(buf, lba, count);
        }
    }
    auto disk_manager::read(disk_fd_t dfd, void *buf, sector_t lba, std::size_t count) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->read(buf, lba, count);
        }
    }
    auto disk_manager::erase(disk_fd_t dfd, sector_t lba, std::size_t count) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        const auto calc_lba = part_lba_to_disk_lba(dfd, lba, count);
        if (calc_lba < 0) {
            return calc_lba;
        }
        else {
            return disk->erase(lba, count);
        }
    }
    auto disk_manager::sync(disk_fd_t dfd) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->sync();
    }
    auto disk_manager::pm_control(disk_fd_t dfd, pm_state target_state) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->pm_control(target_state);
    }
    auto disk_manager::pm_read(disk_fd_t dfd, pm_state &current_state) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return -ENOENT;
        }
        return disk->pm_read(current_state);
    }
    auto disk_manager::status(disk_fd_t dfd) const -> media_status
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return media_status::error;
        }
        return disk->status();
    }
    auto disk_manager::partitions(disk_fd_t dfd) const -> std::vector<partition>
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        return disk->partitions();
    }
    auto disk_manager::get_info(disk_fd_t dfd, info_type what) const -> scount_t
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        return disk->get_info(what);
    }
    auto disk_manager::reread_partitions(disk_fd_t dfd) -> int
    {
        auto disk = dfd->disk().lock();
        if (!disk) {
            LOG_ERROR("Disk doesn't exists");
            return {};
        }
        internal::partition_parser pparser(disk, disk->partitions());
        return pparser.partition_search();
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
} // namespace purefs::blkdev
