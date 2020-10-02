#include "SeveralOptionsItem.hpp"
#include "InputEvent.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/data/OptionParser.hpp"
#include <Style.hpp>
#include <Utils.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{

    SeveralOptionsItem::SeveralOptionsItem(app::Application *app,
                                           const std::string &description,
                                           std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                           std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        application = app;
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, style::window::calendar::item::severalOptions::height);

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        setPenWidth(style::window::default_border_rect_no_focus);
        setMargins(gui::Margins(style::margins::small, style::margins::huge / 2, 0, style::margins::huge / 2));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        vBox->activeItem = false;

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width,
                                         style::window::calendar::item::severalOptions::label_h);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width,
                             style::window::calendar::item::severalOptions::height -
                                 style::window::calendar::item::severalOptions::label_h);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->activeItem = false;

        leftArrow = new gui::Image(hBox, 0, 0, 0, 0);
        leftArrow->setMinimumSize(style::window::calendar::item::severalOptions::arrow_w_h,
                                  style::window::calendar::item::severalOptions::arrow_w_h);
        leftArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        leftArrow->activeItem = false;
        leftArrow->set("arrow_left");

        optionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        optionLabel->setMinimumSize(style::window::default_body_width -
                                        2 * style::window::calendar::item::severalOptions::arrow_w_h,
                                    style::window::calendar::item::severalOptions::height -
                                        style::window::calendar::item::severalOptions::label_h);
        optionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        optionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionLabel->setFont(style::window::font::medium);
        optionLabel->activeItem = false;

        rightArrow = new gui::Image(hBox, 0, 0, 0, 0);
        rightArrow->setMinimumSize(style::window::calendar::item::severalOptions::arrow_w_h,
                                   style::window::calendar::item::severalOptions::arrow_w_h);
        rightArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        rightArrow->activeItem = false;
        rightArrow->set("arrow_right");

        prepareOptionsNames();
        applyCallbacks();
    }

    void SeveralOptionsItem::prepareOptionsNames()
    {
        optionsNames.clear();
        if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_reminder")) {
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_never"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_event_time"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_5_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_15_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_30_min_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_hour_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_2_hour_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_day_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_2_days_before"));
            optionsNames.push_back(utils::localize.get("app_calendar_reminder_1_week_before"));
        }
        else if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_never"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_daily"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_weekly"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_two_weeks"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_month"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_year"));
            optionsNames.push_back(utils::localize.get("app_calendar_repeat_custom"));
        }
    }

    void SeveralOptionsItem::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (item.focus) {
                if (actualVectorIndex == optionsNames.size() - 1 &&
                    descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                    bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"));
                }
            }
            else {
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_LEFT) {
                actualVectorIndex--;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = optionsNames.size() - 1;
                    if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                        bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"));
                    }
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_RIGHT) {
                actualVectorIndex++;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = 0;
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                if (actualVectorIndex == optionsNames.size() - 1 &&
                    descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                    bottomBarTemporaryMode(utils::localize.get("app_calendar_edit"));
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF && actualVectorIndex == optionsNames.size() - 1 &&
                descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                OptionParser parser;
                auto weekDayRepeatData = std::make_unique<WeekDaysRepeatData>();
                assert(weekDayRepeatData != nullptr);
                auto weekDayData = parser.setWeekDayOptions(repeatOptionValue, std::move(weekDayRepeatData));
                application->switchWindow(style::window::calendar::name::custom_repeat_window, std::move(weekDayData));
                return true;
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<EventsRecord> record) {
            if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                if (record->repeat < optionsNames.size() - 1 || actualVectorIndex < optionsNames.size() - 1) {
                    record->repeat = actualVectorIndex;
                }
            }
            else if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_reminder")) {
                record->reminder = static_cast<uint32_t>(reminderTimeOptions[actualVectorIndex]);
            }
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_repeat")) {
                if (event->repeat < optionsNames.size() - 1) {
                    actualVectorIndex = event->repeat;
                }
                else {
                    actualVectorIndex = optionsNames.size() - 1;
                }
                repeatOptionValue = event->repeat;
                optionLabel->setText(optionsNames[actualVectorIndex]);
            }
            else if (descriptionLabel->getText() == utils::localize.get("app_calendar_event_detail_reminder")) {
                actualVectorIndex = std::find(reminderTimeOptions.begin(),
                                              reminderTimeOptions.end(),
                                              static_cast<Reminder>(event->reminder)) -
                                    reminderTimeOptions.begin();
                optionLabel->setText(optionsNames[actualVectorIndex]);
            }
        };
    }

    bool SeveralOptionsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
