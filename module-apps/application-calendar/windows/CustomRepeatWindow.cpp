// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatWindow.hpp"
#include "InputEvent.hpp"
#include "application-calendar/widgets/CheckBoxWithLabelItem.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include <Utils.hpp>
#include <gui/widgets/Window.hpp>

namespace gui
{

    CustomRepeatWindow::CustomRepeatWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::custom_repeat_window),
          customRepeatModel{std::make_shared<CustomRepeatModel>(this->application)}
    {
        buildInterface();
    }

    void CustomRepeatWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void CustomRepeatWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        setTitle(utils::localize.get("app_calendar_custom_repeat_title"));
        list = new gui::ListView(this,
                                 style::window::calendar::listView_x,
                                 style::window::calendar::listView_y,
                                 style::window::calendar::listView_w,
                                 style::window::calendar::listView_h,
                                 customRepeatModel);
        setFocusItem(list);
    }

    void CustomRepeatWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->rebuildList();
        }

        auto recievedData = dynamic_cast<WeekDaysRepeatData *>(data);
        if (recievedData != nullptr) {
            weekDaysOptData = std::make_shared<WeekDaysRepeatData>(*recievedData);
        }
        else {
            weekDaysOptData = std::make_shared<WeekDaysRepeatData>();
        }
        customRepeatModel->loadData(weekDaysOptData);
    }

    bool CustomRepeatWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        // process only if key is released
        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            if (weekDaysOptData != nullptr) {
                auto isCheckedData = customRepeatModel->getIsCheckedData();
                uint32_t i         = 0;
                for (auto checked : isCheckedData) {
                    weekDaysOptData->setData(i, checked);
                    ++i;
                }
                auto data = weekDaysOptData.get();
                application->switchWindow(style::window::calendar::name::new_edit_event,
                                          gui::ShowMode::GUI_SHOW_RETURN,
                                          std::make_unique<WeekDaysRepeatData>(*data));
                return true;
            }
        }
        return false;
    }
} /* namespace gui */
