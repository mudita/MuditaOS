/*
 * DateTimeWindow.hpp
 *
 *  Created on: 28 sty 2020
 *      Author: kuba
 */

#pragma once

#include <functional>
#include <string>

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
        enum class DateTimeItems
        {
            Day,
            Month,
            Year,
            Hour,
            Minute,
        };
        //        enum class TimeItems
        //        {
        //            Hour,
        //            Minute,
        //            AmPm
        //        };
        std::map<DateTimeItems, gui::Item *> dateItems;
        std::map<DateTimeItems, gui::Item *> timeItems;

        gui::HBox *dateBody = nullptr;
        gui::HBox *timeBody = nullptr;

        bool timeFormat12h = false;
        bool timeDateFormat = false; // true europe format, flase american format
        bool dayPeriod = false;      // day period indicator false am, true pm

        gui::Label *addDateTimeItem(Item *parent, const UTF8 &itemTitle, const UTF8 &value);
        gui::Label *addSpacer(const UTF8 &text);
        bool onInput(const InputEvent &inputEvent) override;
        std::string getDateTimeItemValue(DateTimeItems itemType);
        //        std::string getTimeItemValue(TimeItems itemType);
        bool setDate(int32_t value);
        bool setTime(int32_t value);
        void setRTC(void);

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
