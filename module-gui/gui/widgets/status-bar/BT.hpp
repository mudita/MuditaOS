// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "StatusBarWidgetBase.hpp"
#include <service-bluetooth/Constants.hpp>

namespace gui::status_bar
{
    class BT : public StatusBarWidgetBase<Image>
    {
      public:
        BT(Item *parent, uint32_t x, uint32_t y);

        void setBluetoothMode(sys::bluetooth::BluetoothMode mode);
    };
} // namespace gui::status_bar
