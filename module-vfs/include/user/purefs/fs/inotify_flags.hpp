// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace purefs::fs
{
    //! Event monitor flag
    enum class inotify_flags : unsigned
    {
        attrib        = 0x01, //! Attribute changed
        close_write   = 0x02, //! File closed after write
        close_nowrite = 0x04, //! File closed without write
        del           = 0x08, //! File was deleted
        move_src      = 0x10, //! File moved
        move_dst      = 0x20, //! File moved
        open          = 0x40, //! File was opended
        dmodify       = 0x80, //! Directory entry modified
    };
    inline auto operator|(inotify_flags fl1, inotify_flags fl2)
    {
        return static_cast<inotify_flags>(static_cast<unsigned>(fl1) | static_cast<unsigned>(fl2));
    }
    inline auto operator&(inotify_flags fl1, inotify_flags fl2)
    {
        return static_cast<inotify_flags>(static_cast<unsigned>(fl1) & static_cast<unsigned>(fl2));
    }
    inline auto operator&&(inotify_flags fl1, inotify_flags fl2) -> bool
    {
        return static_cast<bool>(static_cast<unsigned>(fl1) & static_cast<unsigned>(fl2));
    }
    inline auto operator||(inotify_flags fl1, inotify_flags fl2) -> bool
    {
        return static_cast<bool>(static_cast<unsigned>(fl1) | static_cast<unsigned>(fl2));
    }

} // namespace purefs::fs
