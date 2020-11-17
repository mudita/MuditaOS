// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include "defs.hpp"
#include "partition.hpp"
#include <list>

namespace purefs::blkdev
{
    class disk;

    /** Disk manager is a class for allows to control block devices media
     */
    class disk_manager
    {
      public:
        disk_manager(const disk_manager &) = delete;
        auto operator=(const disk_manager &) -> disk_manager & = delete;
        /** Register a new disc
         * @param[in] disk Block device register
         * @param[in] device_name Disk friendly name
         * @return zero on success othervise error
         */
        auto register_device(std::shared_ptr<disk> disk, std::string_view device_name) -> int;
        /** Unregister a disc from the manager
         *  param[in] Disc to unregister
         *  @return error code or 0 if success
         */
        auto unregister_device(std::string_view device_name) -> int;
        /** Write a data onto block device or partition
         * @param[in] device_name Block device name or partition name for eg emmc0p1
         * @param[in] buf Data buffer to write
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */
        auto write(std::string_view device_name, const void *buf, sector_t lba, std::size_t count) -> int;
        /** Read a data from block device or partition
         * @param[in] device_name Block device name or partition name for eg emmc0p1
         * @param[in] buf Data buffer for read
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */
        auto read(std::string_view device_name, void *buf, sector_t lba, std::size_t count) -> int;
        /** Erase selected area on the block device or partition
         * @param[in] device_name Block device or partition name
         * @param[in] lba First sector to erase
         * @param[in] count Sectors count for erase
         * @return zero or success otherwise error
         */
        auto erase(std::string_view device_name, sector_t lba, std::size_t count) -> int;
        /** Flush buffers and write all data into the physical device
         * param[in] device_name Partition or device name
         * @return zero or success otherwise error
         */
        auto sync(std::string_view device_name) -> int;
        /** Set block device power state
         * @param[in] device_name Device or partition name
         * @param[in] target_state Set the target power state
         * @return zero or success otherwise error
         * @note If the partition is changed whole device state will be suspended
         */
        auto pm_control(std::string_view device_name, pm_state target_state) -> int;
        /** Get block device power state
         * @param[in] device_name Device or partition name
         * @param[out] currrent_state Device current state
         * @return zero or success otherwise error
         * @note If the partition is changed whole device state will be suspended
         */
        auto pm_read(pm_state &current_state) -> int;
        /** Read the current media status
         * @param[in] device_name Device name or partition
         * @return Current media status @seee media_status
         */
        [[nodiscard]] auto status(std::string_view device_name) const -> media_status;
        /** List the partitions on the underlaying device
         * @param[in] device_name Block device name
         * @return Partition list @see partition
         */
        [[nodiscard]] auto partitions(std::string_view device_name) const -> std::list<partition>;
        /** Get media device info
         * @param[in] device_name Device name or partition
         * @param[in] what Information type @see info_type
         * @return Desired data or error if negative
         */
        [[nodiscard]] auto get_info(std::string_view device_name, info_type what) const -> scount_t;
    };
} // namespace purefs::blkdev
