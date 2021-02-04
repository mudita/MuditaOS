// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NightshiftWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <module-apps/application-settings-new/widgets/SettingsStyle.hpp>
#include <module-apps/widgets/DateAndTimeStyle.hpp>

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
        setTitle(utils::localize.get("app_settings_title_nightshift"));

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        list = new gui::ListView(this,
                                 style::window::date_and_time::listView_x,
                                 style::window::date_and_time::listView_y,
                                 style::window::date_and_time::listView_w,
                                 style::window::date_and_time::listView_h,
                                 timeModel,
                                 style::listview::ScrollBarType::PreRendered);
        setFocusItem(list);

        eventRecord            = std::make_shared<EventsRecord>();
        eventRecord->date_from = TimePointNow();
        timeModel->loadData(eventRecord);
    }

    auto NightshiftWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            timeModel->saveData(eventRecord);
            LOG_DEBUG("Date from: %s", TimePointToLocalizedHourMinString(eventRecord->date_from).c_str());
            LOG_DEBUG("Date to: %s", TimePointToLocalizedHourMinString(eventRecord->date_till).c_str());
            return true;
        }

        return false;
    }
} // namespace gui
