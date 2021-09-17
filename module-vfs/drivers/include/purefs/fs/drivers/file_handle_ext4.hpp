// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <purefs/fs/file_handle.hpp>
#include <ext4_config.h>
#include <ext4.h>

namespace purefs::fs::drivers
{
    class file_handle_ext4 final : public internal::file_handle
    {
      public:
        file_handle_ext4(std::shared_ptr<internal::mount_point> mp, std::string_view path, unsigned flags)
            : file_handle(mp, flags), m_path(path)
        {}
        virtual ~file_handle_ext4() = default;
        [[nodiscard]] auto filp() noexcept
        {
            return &m_file;
        }
        [[nodiscard]] auto open_path() const noexcept -> const std::string &
        {
            return m_path;
        }

      private:
        ::ext4_file m_file{};
        //! Store full path because some handle based fncs are not in ff_Fat
        const std::string m_path;
    };
} // namespace purefs::fs::drivers