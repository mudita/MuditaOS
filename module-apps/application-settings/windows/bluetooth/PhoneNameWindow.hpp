// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>
#include <Text.hpp>

namespace gui
{
    class PhoneNameWindow : public AppWindow
    {
      public:
        explicit PhoneNameWindow(app::ApplicationCommon *app);

      private:
        void buildInterface() override;
        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        Text *inputField = nullptr;
        std::unique_ptr<BluetoothSettingsModel> bluetoothSettingsModel;

        static constexpr auto maxNameLength = 248; // Max 248 bytes according to Bluetooth Core Specification v5.2
    };

} /* namespace gui */
