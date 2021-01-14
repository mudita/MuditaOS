// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Application.hpp>

namespace style
{
    namespace window
    {
        namespace name
        {
            inline constexpr auto event_reminder_window = "EventReminderWindow";
        }
    } // namespace window
} // namespace style

namespace app
{
    inline constexpr auto name_popup = "ApplicationPopup";

    class ApplicationPopup : public Application
    {
      public:
        explicit ApplicationPopup(std::string name                    = name_popup,
                                  std::string parent                  = {},
                                  StartInBackground startInBackground = {false});

        auto InitHandler() -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;
        auto SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes final;

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationPopup>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::ShowReminder}};
        }
    };

} // namespace app
