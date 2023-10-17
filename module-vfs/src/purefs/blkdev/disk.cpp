// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/blkdev/disk.hpp>
#include <errno.h>

namespace purefs::blkdev
{
    auto disk::erase(sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        return -ENOTSUP;
    }
    auto disk::pm_control(pm_state target_state) -> int
    {
        return 0;
    }
    auto disk::pm_read(pm_state &current_state) -> int
    {
        current_state = pm_state::active;
        return 0;
    }
    auto disk::sync() -> int
    {
        return 0;
    }
    auto disk::cleanup() -> int
    {
        return 0;
    }

} // namespace purefs::blkdev
