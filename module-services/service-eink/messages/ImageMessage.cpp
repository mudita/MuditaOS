// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkMessage.hpp"
#include "ImageMessage.hpp"

namespace service::eink
{
    ImageMessage::ImageMessage(
        const std::uint8_t *data, std::uint32_t width, std::uint32_t height, bool deepRefresh, int contextId)
        : data{data}, width{width}, height{height}, deepRefresh{deepRefresh}, contextId{contextId}
    {}

    auto ImageMessage::getData() const noexcept -> const uint8_t *
    {
        return data;
    }

    auto ImageMessage::getSize() const noexcept -> uint32_t
    {
        return width * height;
    }

    auto ImageMessage::isDeepRefresh() const noexcept -> bool
    {
        return deepRefresh;
    }

    auto ImageMessage::getContextId() const noexcept -> int
    {
        return contextId;
    }

    ImageDisplayedNotification::ImageDisplayedNotification(int contextId) : contextId{contextId}
    {}

    auto ImageDisplayedNotification::getContextId() const noexcept -> int
    {
        return contextId;
    }
} /* namespace seink */
