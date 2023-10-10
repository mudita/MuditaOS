// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmTimeItem.hpp"
#include "AlarmClockStyle.hpp"
#include <ListView.hpp>
#include <Label.hpp>
#include <Style.hpp>
#include <time/time_constants.hpp>
#include <time/time_date_validation.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <Utils.hpp>

namespace
{
    constexpr auto maxTimeFieldLength = 2;
}

namespace gui
{
    namespace timeItem = style::alarmClock::window::item::time;

    AlarmTimeItem::AlarmTimeItem(std::function<void(const UTF8 &text)> navBarTemporaryMode,
                                 std::function<void()> navBarRestoreFromTemporaryMode)
        : mode24H(!stm::api::isTimeFormat12h()), navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode))
    {
        setMinimumSize(timeItem::width, timeItem::height);

        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(
            style::widgets::leftMargin, timeItem::marginTop, style::widgets::rightMargin, timeItem::marginBot));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        hourInput = new gui::Text(hBox, 0, 0, 0, 0);
        applyItemSpecificProperties(hourInput);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setMinimumSize(timeItem::separator, timeItem::height);
        colonLabel->setEdges(gui::RectangleEdge::None);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        minuteInput = new gui::Text(hBox, 0, 0, 0, 0);
        applyItemSpecificProperties(minuteInput);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? hBox : nullptr);
            if (!item.focus) {
                if (hourInput->getText().empty()) {
                    hourInput->setText("0");
                }
                hourInput->setText(utils::removeLeadingZeros(hourInput->getText()));
                minuteInput->setText(utils::addLeadingZeros(minuteInput->getText(), maxTimeFieldLength));
            }
            return true;
        };

        applyInputCallbacks();
        prepareForTimeMode();
    }

    void AlarmTimeItem::applyItemSpecificProperties(gui::Text *item)
    {
        item->setEdges(gui::RectangleEdge::Bottom);
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        item->setFont(style::window::font::largelight);
        item->setInputMode(new InputMode({InputMode::digit}));
        item->setPenFocusWidth(style::window::default_border_focus_w);
        item->setPenWidth(style::window::default_border_rect_no_focus);
        item->setEditMode(gui::EditMode::Edit);
    }

    void AlarmTimeItem::applyInputCallbacks()
    {
        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) {
            const auto focusedItem = getFocusItem();
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_ENTER) || event.is(gui::KeyCode::KEY_RF)) {
                return false;
            }

            if (event.isDigit()) {
                clearTimeFieldsIfFull(focusedItem);
            }

            if (focusedItem->onInput(event)) {
                int hours;
                try {
                    hours = std::stoi(hourInput->getText().c_str());
                }
                catch (const std::exception &e) {
                    LOG_ERROR("Hours not valid: %s", e.what());
                    hours = 0;
                }
                if ((mode24H && hours > utils::time::hoursInday - 1) ||
                    (!mode24H && hours > utils::time::hoursInday / 2)) {
                    hourInput->setText(std::to_string(hours % 10));
                }

                int minutes;
                try {
                    minutes = std::stoi(minuteInput->getText().c_str());
                }
                catch (const std::exception &e) {
                    LOG_ERROR("Minutes not valid: %s", e.what());
                    minutes = 0;
                }
                if (minutes > utils::time::minutesInHour - 1) {
                    minuteInput->setText(std::to_string(minutes % 10));
                }

                return true;
            }
            else if (hBox->onInput(event)) {
                switch (event.getKeyCode()) {
                case gui::KeyCode::KEY_RIGHT:
                    if (hourInput->getText().empty()) {
                        hourInput->setText("0");
                    }
                    hourInput->setText(utils::removeLeadingZeros(hourInput->getText()));
                    break;
                case gui::KeyCode::KEY_LEFT:
                    minuteInput->setText(utils::addLeadingZeros(minuteInput->getText(), maxTimeFieldLength));
                    break;
                default:
                    break;
                }
                return true;
            }

            return false;
        };

        onSaveCallback = [&](std::shared_ptr<AlarmEventRecord> record) {
            using namespace utils::time;

            if (hourInput->getText().empty()) {
                hourInput->setText("0");
            }
            if (minuteInput->getText().empty()) {
                minuteInput->setText("00");
            }

            const auto now     = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            const auto newTime = std::localtime(&now);

            if (mode24H) {
                newTime->tm_hour = utils::toNumeric(hourInput->getText());
            }
            else {
                newTime->tm_hour = date::make24(std::chrono::hours{utils::toNumeric(hourInput->getText())},
                                                isPm(mode12hInput->getText()))
                                       .count();
            }
            newTime->tm_min = utils::toNumeric(minuteInput->getText());

            record->alarmTime = AlarmTime{std::chrono::hours{newTime->tm_hour}, std::chrono::minutes{newTime->tm_min}};
        };

        onInputCallback(*hourInput);
        onInputCallback(*minuteInput);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void AlarmTimeItem::onInputCallback(gui::Text &textItem)
    {
        textItem.inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if ((textItem.getText().length() >= maxTimeFieldLength) && !event.is(gui::KeyCode::KEY_LEFT) &&
                !event.is(gui::KeyCode::KEY_RIGHT) && !event.is(gui::KeyCode::KEY_PND) &&
                !event.is(gui::KeyCode::KEY_UP) && !event.is(gui::KeyCode::KEY_DOWN)) {
                return true;
            }
            return false;
        };
    }

    void AlarmTimeItem::prepareForTimeMode()
    {
        if (!mode24H) {
            mode12hInput = new gui::Label(hBox, 0, 0, 0, 0);
            mode12hInput->setEdges(gui::RectangleEdge::Bottom);
            mode12hInput->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            mode12hInput->setFont(style::window::font::largelight);
            mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
            mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
            mode12hInput->setText(utils::translate(utils::time::Locale::getAM()));
            mode12hInput->inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) {
                if (!event.isShortRelease()) {
                    return false;
                }
                if (event.is(gui::KeyCode::KEY_LF)) {
                    if (mode12hInput->getText() == utils::translate(utils::time::Locale::getAM())) {
                        mode12hInput->setText(utils::translate(utils::time::Locale::getPM()));
                    }
                    else {
                        mode12hInput->setText(utils::translate(utils::time::Locale::getAM()));
                    }
                    return true;
                }
                return false;
            };
            mode12hInput->focusChangedCallback = [&](Item &item) {
                if (item.focus) {
                    navBarTemporaryMode(utils::translate("common_switch"));
                    minuteInput->setText(utils::addLeadingZeros(minuteInput->getText(), maxTimeFieldLength));
                }
                else {
                    navBarRestoreFromTemporaryMode();
                }
                return true;
            };

            const auto elemWidth = (timeItem::width - (2 * timeItem::separator)) / 3;
            mode12hInput->setMinimumSize(elemWidth, timeItem::height);
            mode12hInput->setMargins(gui::Margins(timeItem::separator, 0, 0, 0));
            hourInput->setMinimumSize(elemWidth, timeItem::height);
            minuteInput->setMinimumSize(elemWidth, timeItem::height);

            onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
                const auto hours   = alarm->alarmTime.hourOfDay;
                const auto time12H = date::make12(hours);
                const auto isPM    = date::is_pm(hours);

                hourInput->setText(std::to_string(time12H.count()));
                minuteInput->setText(
                    utils::addLeadingZeros(std::to_string(alarm->alarmTime.minuteOfHour.count()), maxTimeFieldLength));

                isPM ? mode12hInput->setText(utils::translate(utils::time::Locale::getPM()))
                     : mode12hInput->setText(utils::translate(utils::time::Locale::getAM()));
            };
        }
        else {
            const auto elemWidth = (timeItem::width - timeItem::separator) / 2;
            hourInput->setMinimumSize(elemWidth, timeItem::height);
            minuteInput->setMinimumSize(elemWidth, timeItem::height);

            onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) {
                hourInput->setText(std::to_string(alarm->alarmTime.hourOfDay.count()));
                minuteInput->setText(
                    utils::addLeadingZeros(std::to_string(alarm->alarmTime.minuteOfHour.count()), maxTimeFieldLength));
            };
        }
    }

    bool AlarmTimeItem::isPm(const std::string &text) const
    {
        return (text != utils::translate(utils::time::Locale::getAM()));
    }

    void AlarmTimeItem::clearTimeFieldsIfFull(Item *focusedItem)
    {
        if ((focusedItem == hourInput) && (hourInput->getText().length() >= maxTimeFieldLength)) {
            hourInput->clear();
        }
        else if ((focusedItem == minuteInput) && (minuteInput->getText().length() >= maxTimeFieldLength)) {
            minuteInput->clear();
        }
    }
} /* namespace gui */
