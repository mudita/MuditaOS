/*
 * DateTimeWindow.hpp
 *
 *  Created on: 28 sty 2020
 *      Author: kuba
 */

#pragma once

#include <functional>
#include <string>

#include "../widgets/DateTimeItem.hpp"
#include "AppWindow.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/Window.hpp"
#include <widgets/BoxLayout.hpp>
namespace gui
{

    /*
     *
     */
    class DateTimeWindow : public AppWindow
    {
      protected:
        enum class DateItems
        {
            Day,
            Month,
            Year
        };
        enum class TimeItems
        {
            Hour,
            Minute,
            AmPm
        };
        std::map<DateItems, gui::Item *> dateItems;
        std::map<TimeItems, gui::Item *> timeItems;

        gui::HBox *dateBody = nullptr;
        gui::HBox *timeBody = nullptr;

        bool timeFormat12h = false;
        bool timeDateFormat = false; // true europe format, flase american format

        gui::Label *addDateTimeItem(Item *parent, const UTF8 &itemTitle, const UTF8 &value, std::function<bool(Item &)> activatedCallback);
        gui::Label *addSpacer(const UTF8 &text);
        bool onInput(const InputEvent &inputEvent) override;
        std::string getDateItemValue(DateItems itemType);
        std::string getTimeItemValue(TimeItems itemType);
        bool setDate(int32_t value);
        bool setTime(int32_t value);

      public:
        DateTimeWindow(app::Application *app);
        virtual ~DateTimeWindow();

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };
} /* namespace gui */
