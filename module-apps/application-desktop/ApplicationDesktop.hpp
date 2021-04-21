// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "windows/Names.hpp"
#include "module-apps/locks/handlers/PinLockHandler.hpp"

#include <Application.hpp>
#include <Service/Message.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>
#include <endpoints/update/UpdateMuditaOS.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <notifications/NotificationListItem.hpp>

namespace cellular
{
    class StateChange;
}

namespace db::query
{
    class ThreadGetCountResult;
    class CalllogGetCountResult;
} // namespace db::query

namespace gui
{
    class NotificationsModel;
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

            Counters notRead;

        } notifications;

        gui::PinLockHandler lockHandler;

        explicit ApplicationDesktop(std::string name                    = name_desktop,
                                    std::string parent                  = {},
                                    sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Connected,
                                    StartInBackground startInBackground = {false});

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
        auto handle(db::query::ThreadGetCountResult *msg) -> bool;
        auto handle(db::query::CalllogGetCountResult *msg) -> bool;
        auto handle(sdesktop::UpdateOsMessage *msg) -> bool;
        auto handle(sdesktop::developerMode::ScreenlockCheckEvent *event) -> bool;
        void handle(manager::actions::NotificationsChangedParams *params) override;
        /**
         * This static method will be used to lock the phone
         */
        //	static bool messageLockPhone( sys::Service* sender, std::string application , const gui::InputEvent& event
        //);
        bool showCalls();
        unsigned int getLockPassHash() const noexcept
        {
            return lockPassHash;
        }
        std::string getOsUpdateVersion() const
        {
            return osUpdateVersion;
        }
        std::string getOsCurrentVersion() const
        {
            return osCurrentVersion;
        }
        void setOsUpdateVersion(const std::string &value);

      private:
        bool refreshMenuWindow();
        void activeSimChanged(std::string value);
        void lockPassHashChanged(std::string value);
        void handleLowBatteryNotification(manager::actions::ActionParamsPtr &&data);
        unsigned int lockPassHash = 0;
        void osUpdateVersionChanged(const std::string &value);
        void osCurrentVersionChanged(const std::string &value);
        std::string osUpdateVersion{updateos::initSysVer};
        std::string osCurrentVersion{updateos::initSysVer};
        std::shared_ptr<gui::NotificationsModel> notificationsModel;
    };

    template <> struct ManifestTraits<ApplicationDesktop>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::AutoLock,
                     manager::actions::RequestPin,
                     manager::actions::RequestPuk,
                     manager::actions::RequestPinChange,
                     manager::actions::RequestPinDisable,
                     manager::actions::RequestPinEnable,
                     manager::actions::RequestScreenPasscode,
                     manager::actions::UnlockSim,
                     manager::actions::BlockSim,
                     manager::actions::ShowMMIResponse,
                     manager::actions::ShowMMIPush,
                     manager::actions::ShowMMIResult,
                     manager::actions::DisplayCMEError,
                     manager::actions::DisplayLowBatteryScreen,
                     manager::actions::SystemBrownout,
                     manager::actions::DisplayLogoAtExit,
                     manager::actions::PhoneModeChanged,
                     manager::actions::NotificationsChanged}};
        }
    };

} /* namespace app */
