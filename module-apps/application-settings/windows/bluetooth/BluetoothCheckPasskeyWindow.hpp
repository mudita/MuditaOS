// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>

namespace gui
{
    class Image;
    class Label;
    class Text;
    class BluetoothCheckPasskeyWindow : public AppWindow
    {
      public:
        BluetoothCheckPasskeyWindow(app::ApplicationCommon *app,
                                    std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel);

      private:
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        Image *image = nullptr;
        Label *label = nullptr;
        Text *text   = nullptr;
        std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel;
    };
} // namespace gui
