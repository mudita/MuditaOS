// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>

namespace purefs::fs::internal
{
    class mount_point;
    class directory_handle
    {
      public:
        directory_handle(std::shared_ptr<mount_point> mp, int error) : m_error(error), m_mount_point(mp)
        {}
        directory_handle(const directory_handle &) = delete;
        auto operator=(const directory_handle &) = delete;
        virtual ~directory_handle()              = default;
        auto error(int error) noexcept -> void
        {
            m_error = error;
        }
        [[nodiscard]] auto error() noexcept
        {
            return m_error;
        }
        [[nodiscard]] auto mntpoint() noexcept
        {
            return m_mount_point.lock();
        }

      private:
        int m_error{};
        const std::weak_ptr<mount_point> m_mount_point;
    };
} // namespace purefs::fs::internal
