// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

#include <gui/Common.hpp>

namespace service::gui
{
    class ChangeColorScheme : public GUIMessage
    {
      public:
        explicit ChangeColorScheme(::gui::ColorScheme colorScheme) : scheme{colorScheme}
        {}

        [[nodiscard]] auto getColorScheme() const noexcept -> ::gui::ColorScheme
        {
            return scheme;
        }

      private:
        ::gui::ColorScheme scheme;
    };
} // namespace service::gui
