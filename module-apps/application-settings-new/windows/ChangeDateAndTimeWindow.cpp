// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangeDateAndTimeWindow.hpp"
#include <application-settings-new/ApplicationSettings.hpp>
#include <gui/input/InputEvent.hpp>
#include <ListView.hpp>
#include <service-evtmgr/service-evtmgr/Constants.hpp>
#include <service-evtmgr/service-evtmgr/EVMessages.hpp>
#include <widgets/DateAndTimeStyle.hpp>

namespace gui
{
    ChangeDateAndTimeWindow::ChangeDateAndTimeWindow(app::Application *app, std::string name)
        : AppWindow(app, name), dateAndTimeModel{std::make_shared<DateAndTimeModel>(this->application)}
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
                                 gui::listview::ScrollBarType::PreRendered);
        setFocusItem(list);

        fromTillDate       = std::make_shared<utils::time::FromTillDate>();
        fromTillDate->from = TimePointNow();
        dateAndTimeModel->loadData(fromTillDate);
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
            dateAndTimeModel->saveData(fromTillDate);
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
        auto msg = std::make_shared<sevm::RtcUpdateTimeMessage>(TimePointToTimeT(fromTillDate->from));
        application->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }
} /* namespace gui */
