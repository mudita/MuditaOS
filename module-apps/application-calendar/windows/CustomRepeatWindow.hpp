#pragma once
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/CustomRepeatModel.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <ListView.hpp>

namespace gui
{
    class CustomRepeatWindow : public gui::AppWindow
    {
        gui::ListView *list                                  = nullptr;
        std::shared_ptr<CustomRepeatModel> customRepeatModel = nullptr;
        std::shared_ptr<WeekDaysRepeatData> weekDaysOptData  = nullptr;

      public:
        CustomRepeatWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void setWeekDaysOptionData(std::unique_ptr<WeekDaysRepeatData> data)
        {
            weekDaysOptData = std::move(data);
        }
    };

} /* namespace gui */
