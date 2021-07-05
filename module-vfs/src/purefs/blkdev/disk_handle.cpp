// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/blkdev/disk.hpp>
#include <log.hpp>

namespace purefs::blkdev::internal
{
    auto disk_handle::sectors() const noexcept -> sector_t
    {
        if (!m_sectors) {
            auto pdisk = m_disk.lock();
            if (pdisk) {
                const auto sector_count = pdisk->get_info(info_type::sector_count, system_partition());
                if (sector_count > 0) {
                    m_sectors = sector_count;
                }
                else {
                    LOG_ERROR("Unable to get sector count.");
                }
            }
            else {
                LOG_ERROR("Unable to get sector count.");
            }
        }
        return m_sectors;
    }
} // namespace purefs::blkdev::internal
