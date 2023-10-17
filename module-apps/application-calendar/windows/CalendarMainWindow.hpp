// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-calendar/ApplicationCalendar.hpp>
#include <models/MonthModel.hpp>
#include <widgets/DayLabel.hpp>
#include <widgets/MonthBox.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/windows/AppWindow.hpp>
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
        std::array<bool, 31> isDayEmpty;
        std::uint32_t offsetFromTop = 0;
        std::uint32_t monthWidth    = 0;
        std::uint32_t monthHeight   = 0;
        std::uint32_t dayWidth      = 0;
        std::uint32_t dayHeight     = 0;
        void refresh();
        void decrementMonthInView();
        void incrementMonthInView();

      protected:
        date::year_month_day actualDate;
        MonthBox *monthBox = nullptr;
        Label *dateLabel   = nullptr;
        std::unique_ptr<MonthModel> monthModel;

      public:
        CalendarMainWindow(app::ApplicationCommon *app, const std::string &name);

        void rebuild() override;
        void buildMonth(std::unique_ptr<MonthModel> &model);
        void buildDateLabel(std::string actualDateTime);
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };

} // namespace gui
