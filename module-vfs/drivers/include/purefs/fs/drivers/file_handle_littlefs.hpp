// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/file_handle.hpp>
#include <lfs.h>

namespace purefs::fs::drivers
{
    class file_handle_littlefs final : public internal::file_handle
    {
      public:
        file_handle_littlefs(std::shared_ptr<internal::mount_point> mp, unsigned flags) : file_handle(mp, flags)
        {}
        virtual ~file_handle_littlefs() = default;
        auto lfs_filp() noexcept
        {
            return &file;
        }

      private:
        ::lfs_file file;
    };

} // namespace purefs::fs::drivers
