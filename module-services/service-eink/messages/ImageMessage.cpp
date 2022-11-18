// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkMessage.hpp"
#include "ImageMessage.hpp"

namespace service::eink
{
    ImageMessage::ImageMessage(int contextId, ::gui::Context *context, ::gui::RefreshModes refreshMode)
        : contextId{contextId}, context{context}, refreshMode{refreshMode}
    {}

    auto ImageMessage::getContext() noexcept -> ::gui::Context *
    {
        return context;
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

} // namespace service::eink
