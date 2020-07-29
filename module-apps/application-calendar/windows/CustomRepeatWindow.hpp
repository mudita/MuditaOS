#pragma once
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/CustomRepeatModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <ListView.hpp>

namespace gui
{
    class CustomRepeatWindow : public gui::AppWindow
    {
        gui::ListView *list                                  = nullptr;
        std::shared_ptr<CustomRepeatModel> customRepeatModel = nullptr;

      public:
        CustomRepeatWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
