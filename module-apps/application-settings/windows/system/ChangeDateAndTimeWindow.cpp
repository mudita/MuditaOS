// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChangeDateAndTimeWindow.hpp"

#include <InputEvent.hpp>
#include <ListView.hpp>
#include <service-time/Constants.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <widgets/DateAndTimeStyle.hpp>

namespace gui
{
    ChangeDateAndTimeWindow::ChangeDateAndTimeWindow(app::ApplicationCommon *app, std::string name)
        : AppWindow(app, name), dateAndTimeModel{std::make_shared<DateAndTimeModel>(this->application)}
    {
        buildInterface();
    }

    void ChangeDateAndTimeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_settings_date_and_time"));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::save));

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

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
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
        auto msg = std::make_shared<stm::message::TimeChangeRequestMessage>(TimePointToTimeT(fromTillDate->from));
        application->bus.sendUnicast(std::move(msg), service::name::service_time);
    }
} /* namespace gui */
