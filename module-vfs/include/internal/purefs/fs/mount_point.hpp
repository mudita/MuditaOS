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
    //! TODO fill the blanks
    struct mount_point
    {
        int device{-1};                                 //! Owning device
        std::string mount_path;                         //! Mounted path
        std::weak_ptr<filesystem_operation> filesystem; //! Filesystem operation
    };
} // namespace purefs::fs::internal
