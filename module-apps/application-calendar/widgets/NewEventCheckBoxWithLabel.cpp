// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEventCheckBoxWithLabel.hpp"
#include "AppWindow.hpp"
#include "InputEvent.hpp"

namespace gui
{

    NewEventCheckBoxWithLabel::NewEventCheckBoxWithLabel(app::Application *application,
                                                         const std::string &description,
                                                         OnCheckCallback onCheck)
        : CheckBoxWithLabelItem(application, description, nullptr), onCheck(std::move(onCheck))
    {
        app = application;
        assert(app != nullptr);

        setMargins(gui::Margins(style::margins::small,
                                style::window::calendar::item::checkBox::margin_top,
                                0,
                                style::window::calendar::leftMargin));
        applyCallbacks();
    }

    void NewEventCheckBoxWithLabel::applyCallbacks()
    {
        inputCallback = [this](gui::Item &item, const gui::InputEvent &event) {
            if (event.is(gui::KeyCode::KEY_LF) && event.isShortPress()) {
                onCheck(checkBox->isChecked());
            }
            return checkBox->onInput(event);
        };

        onLoadCallback = [this](std::shared_ptr<EventsRecord> event) {
            if (allDayEvents::isAllDayEvent(event->date_from, event->date_till)) {
                checkBox->setImageVisible(true);
            }
        };
        onSaveCallback = [this](std::shared_ptr<EventsRecord> event) {
            if (checkBox->isChecked()) {
                event->date_from = TimePointFromYearMonthDay(dateItem->getChosenDate());
                event->date_till = event->date_from + std::chrono::hours(utils::time::Locale::max_hour_24H_mode) +
                                   std::chrono::minutes(utils::time::Locale::max_minutes);
            }
        };
    }

    void NewEventCheckBoxWithLabel::setConnectionToDateItem(gui::DateWidget *item)
    {
        dateItem = item;
    }

    bool allDayEvents::isAllDayEvent(TimePoint start, TimePoint end)
    {
        auto startTime = TimePointToHourMinSec(start);
        auto endTime   = TimePointToHourMinSec(end);
        return startTime.hours().count() == 0 && startTime.minutes().count() == 0 &&
               endTime.hours().count() == utils::time::Locale::max_hour_24H_mode &&
               endTime.minutes().count() == utils::time::Locale::max_minutes;
    }

} /* namespace gui */
