// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

#include <gui/Common.hpp>

namespace service::gui
{
    class RenderingFinished : public GUIMessage
    {
      public:
        RenderingFinished(int contextId, ::gui::RefreshModes refreshMode)
            : contextId{contextId}, refreshMode{refreshMode}
        {}

        [[nodiscard]] int getContextId() const noexcept
        {
            return contextId;
        }

        [[nodiscard]] ::gui::RefreshModes getRefreshMode() const noexcept
        {
            return refreshMode;
        }

      private:
        int contextId;
        ::gui::RefreshModes refreshMode;
    };
} // namespace service::gui
