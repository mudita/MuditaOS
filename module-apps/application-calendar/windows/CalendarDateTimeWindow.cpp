// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarDateTimeWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include <InputEvent.hpp>

namespace gui
{

    CalendarDateTimeWindow::CalendarDateTimeWindow(app::Application *app)
        : AppWindow(app, style::window::calendar::name::date_time_window),
          dateTimeModel{std::make_shared<DateTimeModel>(this->application)}
    {
        buildInterface();
    }

    void CalendarDateTimeWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void CalendarDateTimeWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        list = new gui::ListView(this,
                                 style::window::calendar::listView_x,
                                 style::window::calendar::listView_y,
                                 style::window::calendar::listView_w,
                                 style::window::calendar::listView_h,
                                 dateTimeModel);
        setFocusItem(list);
    }

    void CalendarDateTimeWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (auto rec = dynamic_cast<EventRecordData *>(data); rec != nullptr) {
            eventRecord = rec->getData();
            setTitle(rec->getDescription());
            if (rec->getDescription() == utils::localize.get("app_calendar_end_date")) {
                dateTimeType = DateTimeType::End;
            }
        }
        else {
            eventRecord = std::make_shared<EventsRecord>(EventsRecord());
        }
        dateTimeModel->loadData(eventRecord, dateTimeType);
    }

    bool CalendarDateTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            auto dateTimeData =
                std::make_unique<DateTimeData>(dateTimeModel->getChosenDate(eventRecord, dateTimeType), dateTimeType);
            application->switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_RETURN, std::move(dateTimeData));
            return true;
        }

        return false;
    }
} /* namespace gui */
