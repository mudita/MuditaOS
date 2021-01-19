// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/widgets/DayLabel.hpp"
#include "application-calendar/widgets/MonthBox.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <map>
#include <vector>
#include <string>

namespace db
{
    class QueryResult;
}

namespace gui
{

    class CalendarMainWindow : public gui::AppWindow, public app::AsyncCallbackReceiver
    {
        bool isDayEmpty[31];
        uint32_t offsetFromTop = 0;
        uint32_t monthWidth    = 0;
        uint32_t monthHeight   = 0;
        uint32_t dayWidth      = 0;
        uint32_t dayHeight     = 0;

      protected:
        date::year_month_day actualDate;
        MonthBox *monthBox = nullptr;
        Label *dateLabel = nullptr;
        std::unique_ptr<MonthModel> monthModel;

      public:
        CalendarMainWindow(app::Application *app, const std::string &name);

        void rebuild() override;
        void refresh();
        void filterRequest();
        void buildMonth(std::unique_ptr<MonthModel> &model);
        void buildDateLabel(std::string actualDateTime);
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleQueryResponse(db::QueryResult *queryResult) -> bool;
        std::unique_ptr<MonthModel> getMonthModel()
        {
            return std::move(monthModel);
        }
        bool returnedFromWindow   = false;
        uint32_t dayFocusedBefore = 0;
    };

} // namespace gui
