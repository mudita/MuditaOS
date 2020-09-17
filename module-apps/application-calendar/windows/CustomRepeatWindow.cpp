#include "CustomRepeatWindow.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "application-calendar/widgets/CheckBoxWithLabelItem.hpp"
#include <gui/widgets/Window.hpp>
#include <Utils.hpp>

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

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

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
        if (Window::onInput(inputEvent)) {
            return true;
        }
        // process only if key is released
        if (!inputEvent.isShortPress())
            return false;

        switch (inputEvent.keyCode) {
        case KeyCode::KEY_RF: {
            if (weekDaysOptData != nullptr) {
                auto items = customRepeatModel->getInternalData();
                uint32_t i = 0;
                for (auto it : items) {
                    auto item = dynamic_cast<CheckBoxWithLabelItem *>(it);
                    if (item && item->checkBox->isChecked()) {
                        weekDaysOptData->setData(i);
                    }
                    ++i;
                }
                auto data = weekDaysOptData.get();
                application->switchWindow(style::window::calendar::name::new_edit_event,
                                          gui::ShowMode::GUI_SHOW_RETURN,
                                          std::make_unique<WeekDaysRepeatData>(*data));
                return true;
            }
            else {
                application->returnToPreviousWindow();
            }
        }
        default:
            break;
        }

        return false;
    }

} /* namespace gui */
