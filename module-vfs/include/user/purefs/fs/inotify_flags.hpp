// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace purefs::fs
{
    //! Event monitor flag
    enum class inotify_flags : unsigned
    {
        mattrib        = 0x01, //! Attribute changed
        mclose_write   = 0x02, //! File closed after write
        mclose_nowrite = 0x04, //! File closed without write
        mdelete        = 0x08, //! File was deleted
        mmove          = 0x10, //! File moved
        mopen          = 0x20  //! File was opended
    };
    inline auto operator|(inotify_flags fl1, inotify_flags fl2)
    {
        return static_cast<inotify_flags>(static_cast<unsigned>(fl1) | static_cast<unsigned>(fl2));
    }
    inline auto operator&&(inotify_flags fl1, inotify_flags fl2)
    {
        return static_cast<bool>(static_cast<unsigned>(fl1) & static_cast<unsigned>(fl2));
    }
} // namespace purefs::fs