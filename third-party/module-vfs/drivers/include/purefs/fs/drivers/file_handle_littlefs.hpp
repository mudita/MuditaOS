// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/file_handle.hpp>
#include <lfs.h>

namespace purefs::fs::drivers
{
    class file_handle_littlefs final : public internal::file_handle
    {
      public:
        file_handle_littlefs(std::shared_ptr<internal::mount_point> mp, std::string_view path, unsigned flags)
            : file_handle(mp, flags), m_path(path)
        {}
        virtual ~file_handle_littlefs() = default;
        auto lfs_filp() noexcept
        {
            return &file;
        }
        auto open_path() const noexcept -> const std::string &
        {
            return m_path;
        }

      private:
        ::lfs_file file;
        //! Store full path because some handle based fncs are not in ff_Fat
        const std::string m_path;
    };

} // namespace purefs::fs::drivers
