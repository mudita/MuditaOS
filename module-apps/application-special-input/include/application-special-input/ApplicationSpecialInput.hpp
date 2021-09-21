// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <SwitchData.hpp>
#include <AppWindow.hpp>

namespace app
{

    inline constexpr auto special_input = "ApplicationSpecialInput";
    inline constexpr auto char_select   = gui::name::window::main_window;

    // app just to provide input selection on UI
    class ApplicationSpecialInput : public app::Application
    {
      public:
        std::string requester = "";

        ApplicationSpecialInput(std::string name                    = special_input,
                                std::string parent                  = {},
                                StatusIndicators statusIndicators   = StatusIndicators{},
                                StartInBackground startInBackground = {true});
        virtual ~ApplicationSpecialInput() = default;

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationSpecialInput>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::ShowSpecialInput,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged}};
        }
    };
}; // namespace app
