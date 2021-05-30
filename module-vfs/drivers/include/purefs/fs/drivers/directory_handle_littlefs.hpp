// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/directory_handle.hpp>
#include <lfs.h>

namespace purefs::fs::drivers
{

    class directory_handle_littlefs final : public internal::directory_handle
    {
      public:
        directory_handle_littlefs(std::shared_ptr<internal::mount_point> mp, int error)
            : internal::directory_handle(mp, error)
        {}
        virtual ~directory_handle_littlefs() = default;
        auto lfs_dirp() noexcept
        {
            return &m_directory;
        }

      private:
        ::lfs_dir m_directory{};
    };
} // namespace purefs::fs::drivers
