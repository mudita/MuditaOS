// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
