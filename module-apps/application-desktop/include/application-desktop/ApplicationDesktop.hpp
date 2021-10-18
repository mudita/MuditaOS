// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Constants.hpp"
#include "DBNotificationsHandler.hpp"
#include "Names.hpp"

#include <Application.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <Service/Message.hpp>

namespace gui
{
    class NotificationsModel;
}

namespace app
{
    class ApplicationDesktop : public Application, public AsyncCallbackReceiver
    {
      public:
        explicit ApplicationDesktop(std::string name                    = name_desktop,
                                    std::string parent                  = {},
                                    StatusIndicators statusIndicators   = StatusIndicators{},
                                    StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
        // if there is modem notification and there is no default SIM selected, then we need to select if when unlock is
        // done
        void handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams) override;

        void setOsUpdateVersion(const std::string &value);

      private:
        bool refreshMenuWindow();
        void handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data);
        DBNotificationsHandler dbNotificationHandler;
        sys::MessagePointer handleAsyncResponse(sys::ResponseMessage *resp) override;
    };

    template <> struct ManifestTraits<ApplicationDesktop>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::AutoLock,
                     manager::actions::ShowMMIResponse,
                     manager::actions::ShowMMIPush,
                     manager::actions::ShowMMIResult,
                     manager::actions::DisplayLowBatteryScreen,
                     manager::actions::SystemBrownout,
                     manager::actions::DisplayLogoAtExit,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::NotificationsChanged,
                     manager::actions::AlarmClockStatusChanged}};
        }
    };

} /* namespace app */
