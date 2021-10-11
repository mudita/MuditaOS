﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <cstdint>
#include <module-gui/gui/core/Context.hpp>
#include <module-gui/gui/Common.hpp>

namespace service::eink
{
    class ImageMessage : public EinkMessage
    {
      public:
        ImageMessage(int contextId, ::gui::Context *context, ::gui::RefreshModes refreshMode);

        [[nodiscard]] auto getContextId() const noexcept -> int;
        [[nodiscard]] auto getData() noexcept -> std::uint8_t *;
        [[nodiscard]] auto getRefreshMode() const noexcept -> ::gui::RefreshModes;

      private:
        int contextId;
        ::gui::Context *context;
        ::gui::RefreshModes refreshMode;
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
