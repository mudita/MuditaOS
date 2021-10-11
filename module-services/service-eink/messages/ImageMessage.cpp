﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkMessage.hpp"
#include "ImageMessage.hpp"

namespace service::eink
{
    ImageMessage::ImageMessage(int contextId, ::gui::Context *context, ::gui::RefreshModes refreshMode)
        : contextId{contextId}, context{context}, refreshMode{refreshMode}
    {}

    auto ImageMessage::getData() noexcept -> std::uint8_t *
    {
        return context->getData();
    }

    auto ImageMessage::getRefreshMode() const noexcept -> ::gui::RefreshModes
    {
        return refreshMode;
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
