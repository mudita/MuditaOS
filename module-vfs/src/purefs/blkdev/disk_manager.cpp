// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk.hpp>
#include <log/log.hpp>
#include <errno.h>
#include <charconv>
#include <tuple>
#include <purefs/blkdev/disk_handle.hpp>

/** NOTE: Device manager implementation without sector caching
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
                const auto ret = disk->probe(flags);
                if (ret < 0) {
                    LOG_ERROR("Unable to probe the disc errno %i", ret);
                    return ret;
                }
                const auto fd = m_dev_fds.insert(disk);
                m_dev_map.emplace(std::make_pair(device_name, fd));
                return reread_partitions(std::make_shared<internal::disk_handle>(fd));
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
        const auto ret = m_dev_fds[it->second]->cleanup();
        if (ret < 0) {
            LOG_ERROR("Disk cleanup error %i", ret);
        }
        m_dev_fds.remove(it->second);
        m_dev_map.erase(it);
        return ret;
    }

    auto disk_manager::device_handle(std::string_view device_name) -> disk_fd_t
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

    auto parse_device_name(std::string_view device) -> std::tuple<std::string_view, short>
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
} // namespace purefs::blkdev
