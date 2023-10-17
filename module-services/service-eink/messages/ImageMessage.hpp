// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <hal/eink/AbstractEinkDisplay.hpp>
#include <module-gui/gui/core/Context.hpp>
#include <module-gui/gui/Common.hpp>

#include <cstdint>

namespace service::eink
{
    class ImageMessage : public EinkMessage
    {
      public:
        ImageMessage(int contextId, ::gui::Context *context, ::gui::RefreshModes refreshMode);

        [[nodiscard]] auto getContextId() const noexcept -> int;
        [[nodiscard]] auto getContext() noexcept -> ::gui::Context *;
        [[nodiscard]] auto getRefreshMode() const noexcept -> ::gui::RefreshModes;

      private:
        int contextId;
        ::gui::Context *context;
        ::gui::RefreshModes refreshMode;
    };

    class ShutdownImageMessage : public ImageMessage
    {
      public:
        using ImageMessage::ImageMessage;
    };

    class ImageDisplayedNotification : public EinkMessage
    {
      public:
        explicit ImageDisplayedNotification(int contextId);

        [[nodiscard]] auto getContextId() const noexcept -> int;

      private:
        int contextId;
    };

    class RefreshMessage : public EinkMessage
    {
      public:
        RefreshMessage(int contextId,
                       hal::eink::EinkFrame refreshBox,
                       hal::eink::EinkRefreshMode refreshMode,
                       const std::string &originalSender);

        [[nodiscard]] auto getContextId() const noexcept -> int;
        [[nodiscard]] auto getRefreshFrame() noexcept -> hal::eink::EinkFrame;
        [[nodiscard]] auto getRefreshMode() const noexcept -> hal::eink::EinkRefreshMode;
        [[nodiscard]] auto getOriginalSender() const noexcept -> const std::string &;

      private:
        int contextId;
        hal::eink::EinkFrame refreshFrame;
        hal::eink::EinkRefreshMode refreshMode;
        std::string originalSender;
    };

    class CancelRefreshMessage : public EinkMessage
    {};

} // namespace service::eink
