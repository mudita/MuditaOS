// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "defs.hpp"
#include "partition.hpp"
#include <vector>

namespace purefs::blkdev
{
    class disk
    {
      public:
        disk(const disk &) = delete;
        auto operator=(const disk &) = delete;
        disk()                       = default;
        virtual ~disk()              = default;

        /** Initialize the disk this method is called by the disc manager
         * @param flags Flags passed to the disc manager
         * @return zero on success otherwise error
         */
        virtual auto probe(unsigned flags) -> int = 0;

        /** Disk cleanup just before unregistering
         * @return  zero on success otherwise error
         */
        virtual auto cleanup() -> int = 0;

        /** Write a data onto block device or partition
         * @param[in] buf Data buffer to write
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */
        virtual auto write(const void *buf, sector_t lba, std::size_t count) -> int = 0;

        /** Read a data from block device or partition
         * @param[in] buf Data buffer for read
         * @param[in] lba First sector
         * @param[in] Count sectors count
         * @return zero on success otherwise error
         */
        virtual auto read(void *buf, sector_t lba, std::size_t count) -> int = 0;

        /** Erase selected area on the block device or partition
         * @param[in] lba First sector to erase
         * @param[in] count Sectors count for erase
         * @return zero or success otherwise error
         */
        virtual auto erase(sector_t lba, std::size_t count) -> int;

        /** Flush buffers and write all data into the physical device
         * @return zero or success otherwise error
         */
        virtual auto sync() -> int = 0;

        /** Set block device power state
         * @param[in] target_state Set the target power state
         * @return zero or success otherwise error
         * @note If the partition is changed whole device state will be suspended
         */
        virtual auto pm_control(pm_state target_state) -> int;

        /** Get block device power state
         * @param[out] currrent_state Device current state
         * @return zero or success otherwise error
         * @note If the partition is changed whole device state will be suspended
         */
        virtual auto pm_read(pm_state &current_state) -> int;

        /** Read the current media status
         * @param[in] device_name Device name or partition
         * @return Current media status @seee media_status
         */
        [[nodiscard]] virtual auto status() const -> media_status = 0;

        /** List the partitions on the underlaying device
         * @param[in] device_name Block device name
         * @return Partition list @see partition
         */
        [[nodiscard]] virtual auto get_info(info_type what) const -> scount_t = 0;

        /** List the partitions on the underlaying device
         * @param[in] device_name Block device name
         * @return Partition list @see partition
         */
        [[nodiscard]] auto partitions() -> std::vector<partition> &
        {
            return m_partitions;
        }

      protected:
        /** Clear all partitions */
        auto clear_partitions() -> void
        {
            m_partitions.clear();
        }

      private:
        std::vector<partition> m_partitions;
    };
} // namespace purefs::blkdev
