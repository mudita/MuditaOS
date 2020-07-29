#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/models/DayModel.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <map>
#include <vector>
#include <string>

namespace gui
{
    class CalendarMainWindow;

    namespace coords
    {
        const uint8_t first_row        = 1;
        const uint8_t default_last_row = 5;

        const uint8_t first_column = 0;
        const uint8_t last_column  = 6;
    }; // namespace coords

    class DayLabel : public Label, public DayModel
    {
      public:
        DayLabel(gui::Item *parent,
                 std::string number,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &width,
                 const uint32_t &height);

        void setLabel(std::string number, std::function<bool(Item &)> activatedCallback);
        ~DayLabel() override = default;
    };

    class MonthBox : public GridLayout, public MonthModel
    {
        /// @param columns: defines number of columns, @param rows: defines number of rows
        uint32_t firstRow    = 0;
        uint32_t firstColumn = 0;
        uint32_t columns     = 0;
        uint32_t rows        = 0;
        std::map<std::pair<uint32_t, uint32_t>, DayLabel *> dayMap;

      public:
        MonthBox(app::Application *app,
                 gui::Item *parent,
                 const int &offsetTop,
                 const uint32_t &width,
                 const uint32_t &height,
                 const uint32_t &dayWidth,
                 const uint32_t &dayHeight,
                 const std::unique_ptr<MonthModel> &model);

        ~MonthBox() override = default;

        void buildMap(app::ApplicationCalendar *app);
    };

    class CalendarMainWindow : public gui::AppWindow
    {
        uint32_t offsetFromTop = 0;
        uint32_t monthWidth    = 0;
        uint32_t monthHeight   = 0;
        uint32_t dayWidth      = 0;
        uint32_t dayHeight     = 0;

      protected:
        MonthBox *month = nullptr;
        Label *dateLabel = nullptr;
        std::unique_ptr<MonthModel> monthModel;

      public:
        CalendarMainWindow(app::Application *app, std::string name);

        /// used only for testing
        bool getData(const uint32_t &date_time);

        ~CalendarMainWindow() override = default;
        void switchToNoEventsWindow();
        void rebuild() override;
        void refresh(const uint32_t &ID, std::string date);
        void buildMonth(const uint32_t &actualDateTimeID);
        void buildDateLabel(std::string actualDateTime);
        void buildInterface(const uint32_t &actualDateTimeID);
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };

} // namespace gui
