// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include <Label.hpp>

#include <EventStore.hpp>
#include <gui/widgets/StatusBar.hpp>

namespace gui::status_bar
{
    /// widget drawn on the status bar showing current phone mode:
    class PhoneMode : public StatusBarWidgetBase<Label>
    {

      public:
        /// Phone mode widget class constructor
        /// @param parent parent item pointer
        /// @param x widget x position
        /// @param y widget y position
        /// @param w widget width
        /// @param h widget height
        PhoneMode(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /// Set the current phone mode
        /// @param mode desired mode
        void setPhoneMode(sys::phone_modes::PhoneMode mode);
    };

} // namespace gui::status_bar
