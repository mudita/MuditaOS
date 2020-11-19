// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <string>

namespace purefs::blkdev
{
    class disk;
}

namespace purefs::fs
{
    class filesystem_operation;
}

namespace purefs::fs::internal
{
    //! Mount point disk private structure
    struct mount_point
    {
        std::weak_ptr<blkdev::disk> device; //! Pointer to the device
        int partition;

        std::string mount_path; //! Mounted path
        std::shared_ptr<filesystem_operation> filesystem;
    };
} // namespace purefs::fs::internal
