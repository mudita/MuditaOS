// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/directory_handle.hpp>
#include <redfs.h>
#include <redposix.h>

namespace purefs::fs::drivers
{
    class directory_handle_reedgefs final : public internal::directory_handle
    {
      public:
        directory_handle_reedgefs(std::shared_ptr<internal::mount_point> mp, int error)
            : internal::directory_handle(mp, error)
        {}
        virtual ~directory_handle_reedgefs() = default;
        auto reedgefs_dirp() const noexcept
        {
            return m_dir;
        }
        auto reedgefs_dirp(::REDDIR *dir) noexcept
        {
            m_dir = dir;
        }

      private:
        ::REDDIR *m_dir{};
    };
} // namespace purefs::fs::drivers
