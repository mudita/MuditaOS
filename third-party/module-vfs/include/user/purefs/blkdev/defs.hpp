// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <cstdint>
#include <memory>

namespace purefs::blkdev
{
    //! Unsigned sector type
    using sector_t = uint64_t;
    //! Signed sector type
    using scount_t = int64_t;
    //! Hardware partition type
    using hwpart_t = uint8_t;
    //! Common partition type
    using part_t = int16_t;
    //! Default hardware partition
    constexpr hwpart_t default_hw_partition = 0;
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
        sector_count, //! Number of sectors on disk or part
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

    //! Disk manager flags
    struct flags
    {
        enum _flags
        {
            no_parts_scan = 0x1 //! Don't scan partitions on disc
        };
    };
} // namespace purefs::blkdev
