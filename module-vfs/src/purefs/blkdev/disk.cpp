// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk.hpp>
#include <errno.h>

namespace purefs::blkdev
{
    auto disk::erase(sector_t lba, std::size_t count) -> int
    {
        return -ENOTSUP;
    }
    auto disk::pm_control(pm_state target_state) -> int
    {
        return -ENOTSUP;
    }
    auto disk::pm_read(pm_state &current_state) -> int
    {
        return -ENOTSUP;
    }
} // namespace purefs::blkdev
