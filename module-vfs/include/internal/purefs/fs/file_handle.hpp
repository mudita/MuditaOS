// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

namespace purefs::fs::internal
{
    class mount_point;
    // File handle used for internal operation
    class file_handle
    {
      public:
        file_handle(const file_handle &) = delete;
        virtual ~file_handle()           = default;
        [[nodiscard]] auto error() const noexcept
        {
            return m_error;
        }
        auto error(int error) noexcept -> void
        {
            m_error = error;
        }
        auto flags(int flags) noexcept -> void
        {
            m_flags = flags;
        }
        [[nodiscard]] auto flags() const noexcept
        {
            return m_flags;
        }
        std::weak_ptr<mount_point> m_mount_point;

      private:
        int m_error{};
        int m_flags{};
    };
} // namespace purefs::fs::internal
