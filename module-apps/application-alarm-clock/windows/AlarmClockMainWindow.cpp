// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockMainWindow.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "windows/DialogMetadata.hpp"
#include "messages/DialogMetadataMessage.hpp"
#include <InputEvent.hpp>
#include <service-appmgr/Controller.hpp>
#include <module-services/service-db/service-db/DBNotificationMessage.hpp>

namespace app::alarmClock
{

    AlarmClockMainWindow::AlarmClockMainWindow(
        app::Application *app, std::unique_ptr<AlarmClockMainWindowContract::Presenter> &&windowPresenter)
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
        leftArrowImage = new gui::Image(
            this, style::alarmClock::window::arrow_x, style::alarmClock::window::arrow_y, 0, 0, "arrow_left");
        plusSignImage =
            new gui::Image(this, style::alarmClock::window::cross_x, style::alarmClock::window::cross_y, 0, 0, "cross");

        alarmsList                       = new gui::ListView(this,
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

        emptyListIcon                       = new gui::Icon(this,
                                      0,
                                      style::header::height,
                                      style::window_width,
                                      style::window_height - ::style::header::height - ::style::footer::height,
                                      "phonebook_empty_grey_circle_W_G",
                                      utils::translate("app_alarm_clock_no_alarms_information"));
        emptyListIcon->focusChangedCallback = [this](gui::Item &) {
            onEmptyList();
            return true;
        };
        emptyListIcon->setVisible(false);

        setFocusItem(alarmsList);
    }

    void AlarmClockMainWindow::destroyInterface()
    {
        erase();
        alarmsList     = nullptr;
        leftArrowImage = nullptr;
        plusSignImage  = nullptr;
        emptyListIcon  = nullptr;
    }

    void AlarmClockMainWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (presenter->isAlarmsListEmpty()) {
            showEmptyIcon();
        }
        else {
            showList();
        }
    }

    bool AlarmClockMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            auto rec                              = new AlarmsRecord();
            rec->time                             = TimePointNow();
            auto event                            = std::make_shared<AlarmsRecord>(*rec);
            std::unique_ptr<AlarmRecordData> data = std::make_unique<AlarmRecordData>(event);
            data->setDescription(style::alarmClock::newAlarm);
            application->switchWindow(style::alarmClock::window::name::newEditAlarm, std::move(data));
            return true;
        }

        return false;
    }

    bool AlarmClockMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr && msgNotification->interface == db::Interface::Name::Alarms) {
            if (msgNotification->dataModified()) {
                alarmsList->rebuildList(gui::listview::RebuildType::InPlace);
            }
            if (presenter->isAlarmsListEmpty()) {
                showEmptyIcon();
            }
            else {
                alarmsList->setVisible(true);
                emptyListIcon->setVisible(false);
                setFocusItem(alarmsList);
            }
            return true;
        }
        return false;
    }

    void AlarmClockMainWindow::showEmptyIcon()
    {
        alarmsList->setVisible(false);
        emptyListIcon->setVisible(true);
        setFocusItem(emptyListIcon);
    }

    void AlarmClockMainWindow::showList()
    {
        alarmsList->rebuildList();
        alarmsList->setVisible(true);
        emptyListIcon->setVisible(false);
        setFocusItem(alarmsList);
    }

    void AlarmClockMainWindow::onEmptyList()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, false);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
    }

    void AlarmClockMainWindow::onListFilled()
    {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
    }
} // namespace app::alarmClock
