// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

#include <service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{
    class AddDeviceWindow : public OptionWindow
    {
      public:
        AddDeviceWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        auto devicesOptionsList() -> std::list<gui::Option>;
        void rebuildOptionList();
        std::vector<Devicei> devices;
    };

    class DeviceData : public SwitchData
    {
        std::vector<Devicei> devices;

      public:
        DeviceData(std::vector<Devicei> devices) : SwitchData(), devices(std::move(devices))
        {}
        auto getDevices() -> const std::vector<Devicei> &
        {
            return devices;
        }
    };
}; // namespace gui
