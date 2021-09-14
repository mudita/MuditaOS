// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/fs/file_handle.hpp>

namespace purefs::fs::drivers
{
    class file_handle_reedgefs final : public internal::file_handle
    {
      public:
        file_handle_reedgefs(std::shared_ptr<internal::mount_point> mp, std::string_view path, unsigned flags)
            : file_handle(mp, flags), m_path(path)
        {}
        virtual ~file_handle_reedgefs() = default;
        auto fd() const noexcept
        {
            return m_fd;
        }
        auto fd(int32_t fd) noexcept -> void
        {
            m_fd = fd;
        }
        auto open_path() const noexcept -> std::string override
        {
            return m_path;
        }

      private:
        int32_t m_fd{};
        //! Store full path because some handle based fncs are not in ff_Fat
        const std::string m_path;
    };
} // namespace purefs::fs::drivers
