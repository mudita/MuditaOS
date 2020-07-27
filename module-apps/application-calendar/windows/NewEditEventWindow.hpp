#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Text.hpp>
#include <gui/widgets/ListItem.hpp>

namespace app
{
    class NewEditEventWindow : public gui::AppWindow
    {
        gui::VBox *eventNameVBox   = nullptr;
        gui::Label *eventNameLabel = nullptr;
        gui::Text *eventNameInput  = nullptr;
        gui::ListItem *checkBox    = nullptr;
        gui::ListItem *startTime   = nullptr;

      public:
        NewEditEventWindow(Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
