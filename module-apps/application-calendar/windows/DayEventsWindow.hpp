#pragma once

#include "application-calendar/models/DayEventsInternalModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "NoEvents.hpp"

#include <ListView.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace gui
{
    class DayEventsWindow : public gui::AppWindow
    {
        std::string dayMonthTitle;
        TimePoint filterFrom;
        gui::Image *leftArrowImage                               = nullptr;
        gui::Image *newDayEventImage                             = nullptr;
        gui::ListView *dayEventsList                             = nullptr;
        std::shared_ptr<DayEventsInternalModel> dayEventsModel   = nullptr;

      public:
        DayEventsWindow(app::Application *app);
        bool handleSwitchData(SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        bool onDatabaseMessage(sys::Message *msgl) override;
        void rebuild() override;
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleQueryResponse(db::QueryResult *queryResult) -> bool;
    };

} /* namespace app */
