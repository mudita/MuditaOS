// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SeveralOptionsItem.hpp"
#include "InputEvent.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "application-calendar/data/OptionParser.hpp"
#include "application-alarm-clock//data/AlarmsData.hpp"
#include <Style.hpp>
#include <Utils.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    namespace optionsItem = style::window::calendar::item::severalOptions;

    SeveralOptionsItem::SeveralOptionsItem(app::Application *app,
                                           const std::string &description,
                                           std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                           std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        application = app;
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, optionsItem::height);

        setEdges(RectangleEdge::Bottom);
        setPenWidth(style::window::default_border_rect_no_focus);
        setMargins(gui::Margins(style::window::calendar::leftMargin, style::margins::huge, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->activeItem = false;

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width, optionsItem::label_h);
        descriptionLabel->setMargins(gui::Margins(0, 0, 0, optionsItem::margin));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, optionsItem::hBox_h);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        leftArrow = new gui::Image(hBox, 0, 0, 0, 0);
        leftArrow->setMinimumSize(optionsItem::arrow_w_h, optionsItem::arrow_w_h);
        leftArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        leftArrow->activeItem = false;
        leftArrow->set("arrow_left");
        leftArrow->setVisible(false);

        optionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        optionLabel->setMinimumSize(optionsItem::option_w, optionsItem::hBox_h);
        optionLabel->setMargins(gui::Margins(optionsItem::arrow_w_h, 0, 0, 0));
        optionLabel->setEdges(gui::RectangleEdge::None);
        optionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        optionLabel->setFont(style::window::font::medium);
        optionLabel->activeItem = false;

        rightArrow = new gui::Image(hBox, 0, 0, 0, 0);
        rightArrow->setMinimumSize(optionsItem::arrow_w_h, optionsItem::arrow_w_h);
        rightArrow->setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        rightArrow->activeItem = false;
        rightArrow->set("arrow_right");
        rightArrow->setVisible(false);

        prepareOptionsNames();
        applyCallbacks();
    }

    void SeveralOptionsItem::prepareOptionsNames()
    {
        optionsNames.clear();
        if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_reminder")) {
            optionsNames.push_back(utils::translate("app_calendar_reminder_never"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_event_time"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_5_min_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_15_min_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_30_min_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_1_hour_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_2_hour_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_1_day_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_2_days_before"));
            optionsNames.push_back(utils::translate("app_calendar_reminder_1_week_before"));
        }
        else if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
            optionsNames.push_back(utils::translate("app_calendar_repeat_never"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_daily"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_weekly"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_two_weeks"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_month"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_year"));
            optionsNames.push_back(utils::translate("app_calendar_repeat_custom"));
        }
    }

    void SeveralOptionsItem::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (item.focus) {
                if (actualVectorIndex == optionsNames.size() - 1 &&
                    descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
                    bottomBarTemporaryMode(utils::translate("app_calendar_edit"));
                }
                optionLabel->setMargins(gui::Margins(0, 0, 0, 0));
            }
            else {
                optionLabel->setMargins(gui::Margins(optionsItem::arrow_w_h, 0, 0, 0));
                bottomBarRestoreFromTemporaryMode();
            }
            leftArrow->setVisible(item.focus);
            rightArrow->setVisible(item.focus);
            hBox->resizeItems();
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_LEFT)) {
                actualVectorIndex--;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = optionsNames.size() - 1;
                    if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
                        bottomBarTemporaryMode(utils::translate("app_calendar_edit"));
                    }
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                return true;
            }
            if (event.is(gui::KeyCode::KEY_RIGHT)) {
                actualVectorIndex++;
                if (actualVectorIndex >= optionsNames.size()) {
                    actualVectorIndex = 0;
                }
                optionLabel->setText(optionsNames[actualVectorIndex]);
                if (actualVectorIndex == optionsNames.size() - 1 &&
                    descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
                    bottomBarTemporaryMode(utils::translate("app_calendar_edit"));
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            }
            if (event.is(gui::KeyCode::KEY_LF) && actualVectorIndex == optionsNames.size() - 1 &&
                descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
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
            if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
                if (record->repeat < optionsNames.size() - 1 && actualVectorIndex != optionsNames.size() - 1) {
                    record->repeat = actualVectorIndex;
                }
                else if (record->repeat == optionsNames.size() - 1 ||
                         optionsNames[optionsNames.size() - 1] == utils::translate("app_calendar_repeat_custom")) {
                    record->repeat = static_cast<uint32_t>(AlarmRepeat::never);
                }
            }
            else if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_reminder")) {
                record->reminder = static_cast<uint32_t>(reminderTimeOptions[actualVectorIndex]);
            }
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_repeat")) {
                if (event->repeat < optionsNames.size() - 1) {
                    actualVectorIndex = event->repeat;
                    if (event->repeat == static_cast<uint32_t>(Repeat::never)) {
                        optionsNames[optionsNames.size() - 1] = utils::translate("app_calendar_repeat_custom");
                    }
                    bottomBarRestoreFromTemporaryMode();
                }
                else {
                    auto parser = CustomRepeatValueParser(event->repeat);
                    if (parser.isCustomValueEveryday()) {
                        actualVectorIndex = static_cast<uint32_t>(Repeat::daily);
                        event->repeat     = actualVectorIndex;
                        bottomBarRestoreFromTemporaryMode();
                        optionsNames[optionsNames.size() - 1] = utils::translate("app_calendar_repeat_custom");
                    }
                    else {
                        actualVectorIndex                     = optionsNames.size() - 1;
                        optionsNames[optionsNames.size() - 1] = parser.getWeekDaysText();
                        if (this->focus) {
                            bottomBarTemporaryMode(utils::translate("app_calendar_edit"));
                        }
                    }
                }
                repeatOptionValue = event->repeat;
                optionLabel->setText(optionsNames[actualVectorIndex]);
            }
            else if (descriptionLabel->getText() == utils::translate("app_calendar_event_detail_reminder")) {
                actualVectorIndex = std::find(reminderTimeOptions.begin(),
                                              reminderTimeOptions.end(),
                                              static_cast<Reminder>(event->reminder)) -
                                    reminderTimeOptions.begin();
                optionLabel->setText(optionsNames[actualVectorIndex]);
            }
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
