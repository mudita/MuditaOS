// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <optional>
#include "defs.hpp"
#include "partition.hpp"

namespace cpp_freertos
{
    class MutexRecursive;
}

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
        disk_manager();
        ~disk_manager();
        /** Register a new disc
         * @param[in] disk Block device register
         * @param[in] device_name Disk friendly name
         * @return zero on success othervise error
         */
        auto register_device(std::shared_ptr<disk> disk, std::string_view device_name, unsigned flags = 0) -> int;
        /** Unregister a disc from the manager
         *  param[in] Disc to unregister
         *  @return error code or 0 if success
         */
        auto unregister_device(std::string_view device_name) -> int;
        /** Write a data onto block device or partition
         * @param[in] dfd Disk manager fd
         * @param[in] buf Data buffer to write
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */
        auto write(disk_fd dfd, const void *buf, sector_t lba, std::size_t count) -> int;
        auto write(std::string_view device_name, const void *buf, sector_t lba, std::size_t count) -> int;
        /** Read a data from block device or partition
         * @param[in] dfd Disk manager fd
         * @param[in] buf Data buffer for read
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */

        auto read(disk_fd dfd, void *buf, sector_t lba, std::size_t count) -> int;
        auto read(std::string_view device_name, void *buf, sector_t lba, std::size_t count) -> int;
        /** Erase selected area on the block device or partition
         * @param[in] dfd Disk manager fd
         * @param[in] lba First sector to erase
         * @param[in] count Sectors count for erase
         * @return zero or success otherwise error
         */

        auto erase(disk_fd dfd, sector_t lba, std::size_t count) -> int;
        auto erase(std::string_view device_name, sector_t lba, std::size_t count) -> int;
        /** Flush buffers and write all data into the physical device
         * param[in] dfd Disc manager fd
         * @return zero or success otherwise error
         */
        auto sync(disk_fd dfd) -> int;
        auto sync(std::string_view device_name) -> int;
        /** Set block device power state
         * @param[in] device_name Device or partition name
         * @param[in] target_state Set the target power state
         * @return zero or success otherwise error
         * @note If the partition is changed the device state will be suspended
         */
        auto pm_control(disk_fd dfd, pm_state target_state) -> int;
        auto pm_control(std::string_view device_name, pm_state target_state) -> int;
        /* Set all block devices registered in the disk manager to target pm state
         * @param[in] target_state Set the target power state
         * @return Error code from the last failed device
         * @note If the partition is changed the device state will be suspended
         */
        auto pm_control(pm_state target_state) -> int;
        /** Get block device power state
         * @param[in] dfd Disk device handle
         * @param[out] currrent_state Device current state
         * @return zero or success otherwise error
         * @note If the partition is changed the device state will be suspended
         */
        auto pm_read(disk_fd dfd, pm_state &current_state) -> int;
        auto pm_read(std::string_view device_name, pm_state &current_state) -> int;
        /** Read the current media status
         * @param[in] dfd Disk manager handle
         * @return Current media status @seee media_status
         */
        [[nodiscard]] auto status(disk_fd dfd) const -> media_status;
        [[nodiscard]] auto status(std::string_view device_name) const -> media_status;
        /** List the partitions on the underlaying device
         * @param[in] dfd Disk manager fd
         * @return Partition list @see partition
         */
        [[nodiscard]] auto partitions(disk_fd dfd) const -> std::vector<partition>;
        [[nodiscard]] auto partitions(std::string_view device_name) const -> std::vector<partition>;

        /** Get the selected partition info
         * @Param[in] Disk manager fd
         * @return Partition info data
         */
        [[nodiscard]] auto partition_info(disk_fd dfd) const -> std::optional<partition>;
        [[nodiscard]] auto partition_info(std::string_view device_name) const -> std::optional<partition>;

        /** Get media device info
         * @param[in] dfd Disk manager handle
         * @param[in] what Information type @see info_type
         * @return Desired data or error if negative
         */
        [[nodiscard]] auto get_info(disk_fd dfd, info_type what) const -> scount_t;
        [[nodiscard]] auto get_info(std::string_view device_name, info_type what) const -> scount_t;

        /** Force reread partition tables
         * @param dfd Disk manager handle
         * @return error code
         */
        auto reread_partitions(disk_fd dfd) -> int;
        auto reread_partitions(std::string_view device_name) -> int;
        /**
         * Return the device object based on friendly name
         * @param device_name  Device name
         * @return device object pointer
         */
        auto device_handle(std::string_view device_name) const -> disk_fd;

        /** Convert disk handle containing partitions to the full disk handle
         * @param disk Disk handle with partitions
         * @return disk handle for whole disc
         */
        static auto disk_handle_from_partition_handle(disk_fd disk) -> disk_fd;

      private:
        static auto parse_device_name(std::string_view device) -> std::tuple<std::string_view, part_t>;
        static auto part_lba_to_disk_lba(disk_fd disk, sector_t part_lba, size_t count) -> scount_t;

      private:
        std::unordered_map<std::string, std::shared_ptr<disk>> m_dev_map;
        std::unique_ptr<cpp_freertos::MutexRecursive> m_lock;
    };
} // namespace purefs::blkdev
