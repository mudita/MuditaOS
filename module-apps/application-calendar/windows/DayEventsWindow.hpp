// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/models/DayEventsModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "NoEvents.hpp"

#include <ListView.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <application-calendar/data/dateCommon.hpp>

namespace gui
{
    class Image;

    class DayEventsWindow : public gui::AppWindow
    {
        std::string dayMonthTitle;
        TimePoint filterFrom;
        gui::Image *leftArrowImage                               = nullptr;
        gui::Image *newDayEventImage                             = nullptr;
        gui::ListView *dayEventsList                             = nullptr;
        std::shared_ptr<DayEventsModel> dayEventsModel           = nullptr;

      public:
        DayEventsWindow(app::Application *app);
        bool handleSwitchData(SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} /* namespace app */
