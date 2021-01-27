// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include "SystemManager/SystemManager.hpp"
#include "gui/widgets/Label.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "NoEvents.hpp"

namespace app
{

    inline constexpr auto name_calendar = "ApplicationCalendar";

    class ApplicationCalendar : public Application
    {
        time_t applicationStartTime       = 0;
        int eventShift                    = 0;
        EquivalentWindow equivalentWindow = EquivalentWindow::EmptyWindow;

      public:
        ApplicationCalendar(std::string name,
                            std::string parent,
                            StartInBackground startInBackground = {false},
                            uint32_t stackDepth                 = 8192,
                            sys::ServicePriority priority       = sys::ServicePriority::Idle);

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
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
        void setEquivalentToEmptyWindow(EquivalentWindow window)
        {
            equivalentWindow = window;
        };
        EquivalentWindow getEquivalentToEmptyWindow() const
        {
            return equivalentWindow;
        };
        int getEventShift()
        {
            return eventShift;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
        void switchToNoEventsWindow(const std::string &title = "", const TimePoint &dateFilter = TimePoint());

        static const std::map<Reminder, const char *> reminderOptions;
        static const std::map<Repeat, const char *> repeatOptions;
    };

    template <> struct ManifestTraits<ApplicationCalendar>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::ShowReminder}};
        }
    };
} /* namespace app */
