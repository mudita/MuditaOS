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

    RefreshMessage::RefreshMessage(int contextId,
                                   hal::eink::EinkFrame refreshFrame,
                                   hal::eink::EinkRefreshMode refreshMode,
                                   const std::string &originalSender)
        : contextId(contextId), refreshFrame(refreshFrame), refreshMode(refreshMode), originalSender(originalSender)
    {}

    auto RefreshMessage::getContextId() const noexcept -> int
    {
        return contextId;
    }
    auto RefreshMessage::getRefreshFrame() noexcept -> hal::eink::EinkFrame
    {
        return refreshFrame;
    }
    auto RefreshMessage::getRefreshMode() const noexcept -> hal::eink::EinkRefreshMode
    {
        return refreshMode;
    }
    auto RefreshMessage::getOriginalSender() const noexcept -> const std::string &
    {
        return originalSender;
    }

} // namespace service::eink
