#pragma once

#include "OptionWindow.hpp"

namespace gui
{
    class BluetoothWindow : public OptionWindow
    {
      public:
        BluetoothWindow(app::Application *app);
        ~BluetoothWindow() override = default;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        bool isBluetoothSwitchOn       = false;
        bool isPhoneVisibilitySwitchOn = false;
        auto bluetoothOptionsList() -> std::list<gui::Option>;
        void switchHandler(bool &switchState);
        void rebuildOptionList();
    };
}; // namespace gui
