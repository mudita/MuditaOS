#pragma once
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/CustomRepeatModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <ListView.hpp>

namespace app
{
    class CustomRepeatWindow : public gui::AppWindow
    {
        gui::ListView *list                                  = nullptr;
        std::shared_ptr<CustomRepeatModel> customRepeatModel = nullptr;

      public:
        CustomRepeatWindow(Application *app, std::string name);

        // bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace app */
