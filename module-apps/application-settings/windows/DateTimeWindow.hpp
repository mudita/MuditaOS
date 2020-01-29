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

namespace gui
{

    /*
     *
     */
    class DateTimeWindow : public AppWindow
    {
      protected:
        std::vector<gui::Item *> dateItems;
        std::vector<gui::Item *> timeItems;
        gui::Item *addDateTimeItem(const UTF8 &itemTitle, const UTF8 &value, std::function<bool(Item &)> activatedCallback);

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
