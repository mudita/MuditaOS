// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <cstdint>

namespace service::eink
{
    class ImageMessage : public EinkMessage
    {
      public:
        ImageMessage(
            const std::uint8_t *data, std::uint32_t width, std::uint32_t height, bool deepRefresh, int contextId);

        [[nodiscard]] auto getData() const noexcept -> const uint8_t *;
        [[nodiscard]] auto getSize() const noexcept -> uint32_t;
        [[nodiscard]] auto isDeepRefresh() const noexcept -> bool;
        [[nodiscard]] auto getContextId() const noexcept -> int;

      private:
        const std::uint8_t *data;
        std::uint32_t width;
        std::uint32_t height;
        bool deepRefresh;
        int contextId;
    };

    class ImageDisplayedNotification : public EinkMessage
    {
      public:
        explicit ImageDisplayedNotification(int contextId);

        [[nodiscard]] auto getContextId() const noexcept -> int;

      private:
        int contextId;
    };
} /* namespace seink */
