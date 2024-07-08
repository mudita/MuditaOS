// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/directory_handle.hpp>
#include <ext4.h>

namespace purefs::fs::drivers
{
    class directory_handle_ext4 final : public internal::directory_handle
    {
      public:
        directory_handle_ext4(std::shared_ptr<internal::mount_point> mp, int error)
            : internal::directory_handle(std::move(mp), error)
        {}
        virtual ~directory_handle_ext4() = default;

        [[nodiscard]] auto dirp() noexcept
        {
            return &m_dir;
        }

      private:
        ::ext4_dir m_dir{};
    };
} // namespace purefs::fs::drivers
