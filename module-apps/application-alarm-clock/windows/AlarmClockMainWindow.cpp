// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockMainWindow.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "log/log.hpp"
#include "windows/DialogMetadata.hpp"
#include "messages/DialogMetadataMessage.hpp"
#include <InputEvent.hpp>
#include <header/AddElementAction.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace app::alarmClock
{

    AlarmClockMainWindow::AlarmClockMainWindow(
        app::ApplicationCommon *app, std::unique_ptr<AlarmClockMainWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    AlarmClockMainWindow::~AlarmClockMainWindow() noexcept
    {
        destroyInterface();
    }

    void AlarmClockMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate(style::strings::common::options));

        setTitle(utils::translate("app_alarm_clock_title_main"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);

        alarmsList = new gui::ListView(this,
                                       style::alarmClock::window::listView_x,
                                       style::alarmClock::window::listView_y,
                                       style::alarmClock::window::listView_w,
                                       style::alarmClock::window::listView_h,
                                       presenter->getAlarmsItemProvider(),
                                       gui::listview::ScrollBarType::Fixed);

        alarmsList->focusChangedCallback = [this](gui::Item &) {
            onListFilled();
            return true;
        };
        alarmsList->rebuildList();

        emptyListIcon =
            new gui::Icon(this,
                          0,
                          style::window::default_vertical_pos,
                          style::window_width,
                          style::window_height - ::style::window::default_vertical_pos - ::style::footer::height,
                          "empty_list_add_W_G",
                          utils::translate("app_alarm_clock_no_alarms_information"));

        alarmsList->emptyListCallback    = [this]() { onEmptyList(); };
        alarmsList->notEmptyListCallback = [this]() { onListFilled(); };

        setFocusItem(alarmsList);
    }

    void AlarmClockMainWindow::destroyInterface()
    {
        erase();
        alarmsList    = nullptr;
        emptyListIcon = nullptr;
    }

    bool AlarmClockMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            auto rec                              = new AlarmEventRecord();
            rec->startDate                        = TimePointNow();
            rec->snoozeDuration                   = 10;
            auto event                            = std::make_shared<AlarmEventRecord>(*rec);
            std::unique_ptr<AlarmRecordData> data = std::make_unique<AlarmRecordData>(event);
            data->setDescription(style::alarmClock::newAlarm);
            application->switchWindow(style::alarmClock::window::name::newEditAlarm, std::move(data));
            return true;
        }

        return false;
    }

    bool AlarmClockMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        // TODO this is actually bad - we can request data from ServiceTime when there is no data there
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr && msgNotification->interface == db::Interface::Name::AlarmEvents) {
            if (msgNotification->dataModified()) {
                alarmsList->rebuildList(gui::listview::RebuildType::InPlace);
                return true;
            }
        }
        return false;
    }

    void AlarmClockMainWindow::onEmptyList()
    {
        emptyListIcon->setVisible(true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void AlarmClockMainWindow::onListFilled()
    {
        emptyListIcon->setVisible(false);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }
} // namespace app::alarmClock
