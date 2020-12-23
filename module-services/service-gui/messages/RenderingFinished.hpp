// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

#include <gui/Common.hpp>

namespace service::gui
{
    class RenderingFinished : public GUIMessage
    {
      public:
        explicit RenderingFinished(int contextId, ::gui::RefreshModes refreshMode)
            : contextId{contextId}, refreshMode{refreshMode}
        {}

        [[nodiscard]] int getContextId() const noexcept
        {
            return contextId;
        }

        [[nodiscard]] bool isDeepRefreshRequested() const noexcept
        {
            return refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP;
        }

      private:
        int contextId;
        ::gui::RefreshModes refreshMode;
    };
} // namespace service::gui
