// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditEventWindow.hpp"
#include <module-db/Interface/EventsRecord.hpp>
#include <service-db/DBServiceAPI.hpp>

namespace gui
{

    NewEditEventWindow::NewEditEventWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::new_edit_event),
          newEditEventModel{std::make_shared<NewEditEventModel>(this->application)}
    {
        buildInterface();
    }

    void NewEditEventWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void NewEditEventWindow::buildInterface()
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
                                 newEditEventModel,
                                 style::listview::ScrollBarType::PreRendered);
        setFocusItem(list);
    }

    void NewEditEventWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        switch (eventAction) {
        case EventAction::Add: {
            setTitle(utils::localize.get("app_calendar_new_event_title"));
            break;
        }
        case EventAction::Edit:
            setTitle(utils::localize.get("app_calendar_edit_event_title"));
            break;
        }

        if (mode == ShowMode::GUI_SHOW_INIT) {
            auto rec = dynamic_cast<EventRecordData *>(data);
            if (rec != nullptr) {
                eventRecord = rec->getData();
            }
            newEditEventModel->loadData(eventRecord);
        }
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            auto receivedData = dynamic_cast<WeekDaysRepeatData *>(data);
            if (receivedData != nullptr) {
                auto parser         = std::make_unique<OptionParser>();
                auto uniqueData     = std::make_unique<WeekDaysRepeatData>(*receivedData);
                eventRecord->repeat = parser->getDatabaseFieldValue(std::move(uniqueData));
                newEditEventModel->loadRepeat(eventRecord);
            }

            if (auto dateTimeData = dynamic_cast<DateTimeData *>(data); dateTimeData != nullptr) {
                if (dateTimeData->getDateTimeType() == DateTimeType::Start) {
                    eventRecord->date_from = dateTimeData->getDateTimeData();
                    handleEndDateUpdate();
                    newEditEventModel->loadStartDate(eventRecord);
                }
                else {
                    eventRecord->date_till = dateTimeData->getDateTimeData();
                    checkEventDurationCorrectness();
                }
            }
        }
        if (eventRecord->date_till - eventRecord->date_from > date::days(1)) {
            newEditEventModel->reloadWithoutRepeat();
            eventRecord->repeat = static_cast<uint32_t>(Repeat::never);
        }
    }

    bool NewEditEventWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            LOG_DEBUG("Save Event");
            newEditEventModel->saveData(eventRecord, eventAction);
            return true;
        }

        return false;
    }

    bool NewEditEventWindow::handleSwitchData(gui::SwitchData *data)
    {
        if (data == nullptr) {
            return false;
        }

        if (data->getDescription() == style::window::calendar::edit_event) {
            eventAction = EventAction::Edit;
        }
        else if (data->getDescription() == style::window::calendar::new_event) {
            eventAction = EventAction::Add;
        }
        return true;
    }

    void NewEditEventWindow::handleEndDateUpdate()
    {
        auto start = TimePointToYearMonthDay(eventRecord->date_from);
        auto end   = TimePointToYearMonthDay(eventRecord->date_till);
        if (start > end) {
            auto hms               = TimePointToHourMinSec(eventRecord->date_till);
            eventRecord->date_till = TimePointFromYearMonthDay(start) + hms.hours() + hms.minutes();
        }
        newEditEventModel->loadEndDate(eventRecord);
    }

    void NewEditEventWindow::checkEventDurationCorrectness()
    {
        auto start = TimePointToYearMonthDay(eventRecord->date_from);
        if (eventRecord->date_till - eventRecord->date_from > date::days(31)) {
            auto hms               = TimePointToHourMinSec(eventRecord->date_till);
            eventRecord->date_till = TimePointFromYearMonthDay(start) + hms.hours() + hms.minutes() + date::days(31);
        }
        newEditEventModel->loadEndDate(eventRecord);
    }
} /* namespace gui */
