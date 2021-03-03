// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
#include "gui/widgets/Window.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui
{

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

        std::map<DateTimeItems, gui::Item *> dateItems;
        std::map<DateTimeItems, gui::Item *> timeItems;

        gui::HBox *dateBody = nullptr;
        gui::HBox *timeBody = nullptr;

        bool dayPeriod      = false; // day period indicator false am, true pm

        gui::Label *addDateTimeItem(Item *parent, const UTF8 &itemTitle, const UTF8 &value);
        gui::Label *addSpacer(const UTF8 &text);
        bool onInput(const InputEvent &inputEvent) override;
        std::string getDateTimeItemValue(DateTimeItems itemType);
        bool setDate(int32_t value);
        bool setTime(int32_t value);
        void setRTC(void);
        bool isEuroTime = false;

      public:
        DateTimeWindow(app::Application *app, bool isEuroTime);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void setEuroTime(bool euroTime);

      private:
        void invalidate() noexcept;
    };
} /* namespace gui */
