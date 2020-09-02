#pragma once

#include "windows/Names.hpp"

#include <Application.hpp>
#include <Service/Message.hpp>
#include <service-cellular/messages/CellularMessage.hpp>
#include <service-db/messages/DBNotificationMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>

namespace app
{
    inline const std::string name_desktop = "ApplicationDesktop";

    class ApplicationDesktop : public Application
    {
      protected:
        // determines whether screen should be protected by pin verification
        bool screenLocked = true;
        bool pinLocked    = false;
        void reloadSettings();

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

        } notifications;

        ApplicationDesktop(std::string name = name_desktop, std::string parent = "", bool startBackground = false);
        virtual ~ApplicationDesktop();
        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bool getScreenLocked();
        void setScreenLocked(bool val);
        bool getPinLocked();
        // if there is modem notification and there is no default SIM selected, then we need to select if when unlock is
        // done
        bool handle(db::NotificationMessage *msg);
        bool handle(cellular::StateChange *msg);
        auto handle(db::query::notifications::GetAllResult *msg) -> bool;
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
    };

} /* namespace app */
