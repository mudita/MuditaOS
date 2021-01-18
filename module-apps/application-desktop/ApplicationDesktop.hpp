// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "windows/Names.hpp"
#include "widgets/PinLockHandler.hpp"

#include <Application.hpp>
#include <Service/Message.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>
#include <endpoints/update/UpdateMuditaOS.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DesktopMessages.hpp>

namespace cellular
{
    class StateChange;
}

namespace app
{
    inline constexpr auto name_desktop = "ApplicationDesktop";

    class ApplicationDesktop : public Application
    {
      public:
        bool need_sim_select = false;
        struct Notifications
        {
            struct Counters
            {
                unsigned int SMS   = 0;
                unsigned int Calls = 0;

                auto areEmpty()
                {
                    return Calls == 0 && SMS == 0;
                }
            };

            Counters notSeen;
            Counters notRead;

            bool batteryLowLevel = false;

        } notifications;

        gui::PinLockHandler lockHandler;

        ApplicationDesktop(std::string name                    = name_desktop,
                           std::string parent                  = {},
                           StartInBackground startInBackground = {false});
        virtual ~ApplicationDesktop();
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
        // if there is modem notification and there is no default SIM selected, then we need to select if when unlock is
        // done
        bool handle(db::NotificationMessage *msg);
        bool handle(cellular::StateChange *msg);
        auto handle(db::query::notifications::GetAllResult *msg) -> bool;
        auto handle(sdesktop::UpdateOsMessage *msg) -> bool;
        auto handle(sdesktop::developerMode::ScreenlockCheckEvent *event) -> bool;
        /**
         * This static method will be used to lock the phone
         */
        //	static bool messageLockPhone( sys::Service* sender, std::string application , const gui::InputEvent& event
        //);
        bool showCalls();
        bool clearCallsNotification();
        bool clearMessagesNotification();
        bool requestNotSeenNotifications();
        bool requestNotReadNotifications();
        unsigned int getLockPassHash() const noexcept
        {
            return lockPassHash;
        }

      private:
        void activeSimChanged(std::string value);
        void lockPassHashChanged(std::string value);
        void handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data);
        unsigned int lockPassHash = 0;
    };

    template <> struct ManifestTraits<ApplicationDesktop>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::RequestPin,
                     manager::actions::RequestPuk,
                     manager::actions::RequestPinChange,
                     manager::actions::UnlockSim,
                     manager::actions::BlockSim,
                     manager::actions::ShowMMIResponse,
                     manager::actions::ShowMMIPush,
                     manager::actions::ShowMMIResult,
                     manager::actions::DisplayCMEError,
                     manager::actions::DisplayLowBatteryNotification}};
        }
    };

} /* namespace app */
