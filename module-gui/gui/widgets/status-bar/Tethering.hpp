// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include <Label.hpp>
#include <gui/widgets/StatusBar.hpp>

namespace gui::status_bar
{
    /// widget drawn on the status bar showing current tethering state:
    class Tethering : public StatusBarWidgetBase<Image>
    {

      public:
        /// Phone mode widget class constructor
        /// @param parent parent item pointer
        /// @param x widget x position
        /// @param y widget y position
        Tethering(Item *parent, std::uint32_t x, std::uint32_t y);
    };

} // namespace gui::status_bar
