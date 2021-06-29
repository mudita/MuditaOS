// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NightshiftWindow.hpp"

#include "ApplicationSettings.hpp"
#include "SettingsStyle.hpp"

#include <gui/input/InputEvent.hpp>
#include <ListView.hpp>
#include <widgets/DateAndTimeStyle.hpp>

namespace gui
{
    NightshiftWindow::NightshiftWindow(app::Application *app)
        : AppWindow(app, gui::window::name::nightshift), timeModel{
                                                             std::make_shared<FromTimeToTimeModel>(this->application)}
    {
        buildInterface();
    }

    void NightshiftWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_settings_title_nightshift"));

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        list = new gui::ListView(this,
                                 style::window::date_and_time::listView_x,
                                 style::window::date_and_time::listView_y,
                                 style::window::date_and_time::listView_w,
                                 style::window::date_and_time::listView_h,
                                 timeModel,
                                 gui::listview::ScrollBarType::PreRendered);
        setFocusItem(list);

        fromTillDate       = std::make_shared<utils::time::FromTillDate>();
        fromTillDate->from = TimePointNow();
        timeModel->loadData(fromTillDate);
    }

    auto NightshiftWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            timeModel->saveData(fromTillDate);
            LOG_DEBUG("Date from: %s", TimePointToLocalizedHourMinString(fromTillDate->from).c_str());
            LOG_DEBUG("Date to: %s", TimePointToLocalizedHourMinString(fromTillDate->till).c_str());
            return true;
        }

        return false;
    }
} // namespace gui
