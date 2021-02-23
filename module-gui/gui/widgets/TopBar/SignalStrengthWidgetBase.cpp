// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthWidgetBase.hpp"

namespace gui
{
    SignalStrengthWidgetBase::SignalStrengthWidgetBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : StatusBarWidgetBase(parent, x, y, w, h)
    {
        setBorderColor(gui::ColorNoColor);
    }

    void SignalStrengthWidgetBase::update(const Store::SignalStrength &data)
    {
        signalStrength = data;
        update();
    }
} // namespace gui
