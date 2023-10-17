// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BT.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    constexpr auto bt_status           = "bt_status";
    constexpr auto bt_connected_status = "bt_connected_status";

    BT::BT(Item *parent, uint32_t x, uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(bt_status, style::status_bar::imageTypeSpecifier);
    }

    void BT::setBluetoothMode(sys::bluetooth::BluetoothMode mode)
    {
        switch (mode) {
        case sys::bluetooth::BluetoothMode::Disabled:
            setVisible(false);
            break;
        case sys::bluetooth::BluetoothMode::Enabled:
            set(bt_status, style::status_bar::imageTypeSpecifier);
            break;
        case sys::bluetooth::BluetoothMode::ConnectedVoice:
            [[fallthrough]];
        case sys::bluetooth::BluetoothMode::ConnectedAudio:
            [[fallthrough]];
        case sys::bluetooth::BluetoothMode::ConnectedBoth:
            set(bt_connected_status, style::status_bar::imageTypeSpecifier);
            break;
        }
    }
}; // namespace gui::status_bar
