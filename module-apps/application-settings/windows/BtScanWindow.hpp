// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <BoxLayout.hpp>
#include <memory>
#include <service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{
    namespace name::window
    {
        inline constexpr auto name_btscan = "BT_SCAN";
    }

    class BtScanWindow : public AppWindow
    {
      protected:
        VBox *box;

      public:
        BtScanWindow(app::Application *app, std::vector<Devicei> devices = {});

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;
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
} // namespace gui
