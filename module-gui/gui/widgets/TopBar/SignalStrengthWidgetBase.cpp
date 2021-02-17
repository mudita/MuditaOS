// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SignalStrengthWidgetBase.hpp"

namespace gui
{
    SignalStrengthWidgetBase::SignalStrengthWidgetBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : HBox(parent, x, y, w, h)
    {
        setBorderColor(gui::ColorNoColor);
    }
} // namespace gui
