// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace purefs::fs::internal
{
    class directory_handle
    {
      public:
        directory_handle(const directory_handle &) = delete;
        virtual ~directory_handle()                = delete;
        auto operator=(const directory_handle &) = delete;
        auto error(int error) noexcept -> void
        {
            m_error = error;
        }
        [[nodiscard]] auto error() noexcept
        {
            return m_error;
        }

      private:
        int m_error{};
    };
} // namespace purefs::fs::internal
