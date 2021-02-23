// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEventCheckBoxWithLabel.hpp"
#include "AppWindow.hpp"

namespace gui
{

    NewEventCheckBoxWithLabel::NewEventCheckBoxWithLabel(app::Application *application,
                                                         const std::string &description,
                                                         NewEditEventModel *model)
        : CheckBoxWithLabelItem(application, description, nullptr), model(model)
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
        focusChangedCallback = [&](Item &item) {
            if (focus) {
                setFocusItem(checkBox);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (checkBox->isChecked() && event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("reloadDataWithTimeItem");
                model->reloadDataWithTimeItem();
            }
            else if (!checkBox->isChecked() && event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("loadDataWithoutTimeItem");
                model->loadDataWithoutTimeItem();
            }

            if (checkBox->onInput(event)) {
                checkBox->resizeItems();
                app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode();
                return true;
            }

            return false;
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            auto start_time = TimePointToHourMinSec(event->date_from);
            auto end_time   = TimePointToHourMinSec(event->date_till);
            if (start_time.hours().count() == 0 && start_time.minutes().count() == 0 &&
                end_time.hours().count() == utils::time::Locale::max_hour_24H_mode &&
                end_time.minutes().count() == utils::time::Locale::max_minutes) {
                checkBox->setImageVisible(true);
            }
        };
        onSaveCallback = [&](std::shared_ptr<EventsRecord> event) {
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

} /* namespace gui */
