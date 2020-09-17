#include "NewEventCheckBoxWithLabel.hpp"

namespace gui
{

    NewEventCheckBoxWithLabel::NewEventCheckBoxWithLabel(app::Application *application,
                                                         const std::string &description,
                                                         bool checkIsOnLeftBarSide,
                                                         NewEditEventModel *model)
        : CheckBoxWithLabelItem(application, description, nullptr, checkIsOnLeftBarSide), model(model)
    {
        app = application;
        assert(app != nullptr);

        setMargins(gui::Margins(
            style::margins::small, style::window::calendar::item::checkBox::margin_top, 0, style::margins::small));
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
                end_time.hours().count() == style::window::calendar::time::max_hour_24H_mode &&
                end_time.minutes().count() == style::window::calendar::time::max_minutes) {
                checkBox->setImageVisible(true);
            }
        };
        onSaveCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (checkBox->isChecked()) {
                auto event_start = TimePointToHourMinSec(event->date_from);
                event->date_from =
                    event->date_from - event_start.hours() - event_start.minutes() - event_start.seconds();
                event->date_till = event->date_from +
                                   std::chrono::hours(style::window::calendar::time::max_hour_24H_mode) +
                                   std::chrono::minutes(style::window::calendar::time::max_minutes);
            }
        };
    }

} /* namespace gui */
