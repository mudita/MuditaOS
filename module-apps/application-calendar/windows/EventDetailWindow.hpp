#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <Text.hpp>
#include <Style.hpp>

namespace app
{
    class EventDetailWindow : public gui::AppWindow
    {
        gui::Text *description      = nullptr;
        gui::Label *time            = nullptr;
        gui::Label *repeat          = nullptr;
        gui::Label *reminder        = nullptr;
        gui::Label *repeatHeading   = nullptr;
        gui::Label *reminderHeading = nullptr;
        gui::Label *eventHeading    = nullptr;

      public:
        EventDetailWindow(Application *app, std::string name);

        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
