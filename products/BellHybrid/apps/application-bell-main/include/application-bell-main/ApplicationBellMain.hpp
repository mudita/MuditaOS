// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

namespace gui::window::name
{
    inline constexpr auto bell_main_menu        = "BellMainMenu";
    inline constexpr auto bell_main_menu_dialog = "BellMainMenuDialog";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto applicationBellName = "ApplicationBell";

    class ApplicationBellMain : public Application
    {
      public:
        explicit ApplicationBellMain(std::string name                    = applicationBellName,
                                     std::string parent                  = "",
                                     StatusIndicators statusIndicators   = StatusIndicators{},
                                     StartInBackground startInBackground = {false},
                                     std::uint32_t stackDepth            = 8192);

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

      private:
        void showPopup(gui::popup::ID id, const gui::PopupRequestParams *params) override;
        auto isHomeScreenFocused() -> bool;
        void idleTimerApplicationStartAction() override;
        sys::MessagePointer handleSwitchWindow(sys::Message *msgl) override;
    };

    template <> struct ManifestTraits<ApplicationBellMain>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::ShowAlarm}};
        }
    };
} // namespace app
