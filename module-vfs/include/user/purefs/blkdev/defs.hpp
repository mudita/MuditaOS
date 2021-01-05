// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <cstdint>
#include <memory>

namespace purefs::blkdev
{
    using sector_t = uint64_t;
    using scount_t = int64_t;
    namespace internal
    {
        class disk_handle;
    }
    using disk_fd = std::shared_ptr<internal::disk_handle>;
    //! Disk status result
    enum class media_status
    {
        healthly, //! Disk OK
        uninit,   //! Disk unintialized
        nomedia,  //! No media
        wprotect, //! Write protected
        error,    //! Internal error
    };
    // Information parameter
    enum class info_type
    {
        sector_count, //! Number of sectors on disk
        sector_size,  //! Single sector size
        erase_block   //! Number of sectors in erase block
    };

    //! Power control states
    enum class pm_state
    {
        active,        //! Device is in active state
        low_power,     //! Device is in low power state
        suspend,       //! Device is in suspend state
        force_suspend, //! Device is in force suspend state
        power_off      //! Device is in poweroff state
    };
} // namespace purefs::blkdev
