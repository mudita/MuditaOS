#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include "SystemManager/SystemManager.hpp"
#include "gui/widgets/Label.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>
#include "application-calendar/widgets/CalendarStyle.hpp"

namespace app
{

    inline const std::string name_calendar = "ApplicationCalendar";

    class ApplicationCalendar : public Application
    {
        time_t applicationStartTime = 0;
        int eventShift              = 0;

      public:
        ApplicationCalendar(std::string name,
                            std::string parent,
                            uint32_t stackDepth           = 4096,
                            sys::ServicePriority priority = sys::ServicePriority::Idle);

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        time_t getCurrentTimeStamp()
        {
            return applicationStartTime;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        void switchToNoEventsWindow(const std::string &title,
                                    const TimePoint &dateFilter,
                                    const std::string &goBackWindow);

        static const std::map<Reminder, const char *> reminderOptions;
        static const std::map<Repeat, const char *> repeatOptions;
    };
} /* namespace app */
