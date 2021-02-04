// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangeDateAndTimeWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <module-apps/widgets/DateAndTimeStyle.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-services/service-evtmgr/service-evtmgr/Constants.hpp>
#include <module-services/service-evtmgr/service-evtmgr/EVMessages.hpp>
#include <service-db/DBServiceAPI.hpp>

namespace gui
{
    ChangeDateAndTimeWindow::ChangeDateAndTimeWindow(app::Application *app)
        : AppWindow(app, gui::window::name::change_date_and_time), dateAndTimeModel{std::make_shared<DateAndTimeModel>(
                                                                       this->application)}
    {
        buildInterface();
    }

    void ChangeDateAndTimeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_date_and_time"));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        list = new gui::ListView(this,
                                 style::window::date_and_time::listView_x,
                                 style::window::date_and_time::listView_y,
                                 style::window::date_and_time::listView_w,
                                 style::window::date_and_time::listView_h,
                                 dateAndTimeModel,
                                 style::listview::ScrollBarType::PreRendered);
        setFocusItem(list);

        eventRecord            = std::make_shared<EventsRecord>();
        eventRecord->date_from = TimePointNow();
        dateAndTimeModel->loadData(eventRecord);
    }

    bool ChangeDateAndTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            dateAndTimeModel->saveData(eventRecord);
            sendRtcUpdateTimeMessage();
            return true;
        }

        return false;
    }

    void ChangeDateAndTimeWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void ChangeDateAndTimeWindow::sendRtcUpdateTimeMessage()
    {
        auto msg = std::make_shared<sevm::RtcUpdateTimeMessage>(TimePointToTimeT(eventRecord->date_from));
        application->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }
} /* namespace gui */
