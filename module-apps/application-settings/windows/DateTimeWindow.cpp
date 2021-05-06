// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * DateTimeWindow.cpp
 *
 *  Created on: 28 sty 2020
 *      Author: kuba
 */

#include <functional>
#include <memory>

#include <service-appmgr/model/ApplicationManager.hpp>

#include "../ApplicationSettings.hpp"
#include "DateTimeWindow.hpp"

#include <i18n/i18n.hpp>
#include "time/time_conversion.hpp"
#include "time/time_date_validation.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "Text.hpp"
#include <Style.hpp>
#include <widgets/BoxLayout.hpp>

#include <module-utils/time/DateAndTimeSettings.hpp>
#include <module-utils/Utils.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>

namespace gui
{

    DateTimeWindow::DateTimeWindow(app::Application *app, bool euroTime)
        : AppWindow(app, "DateTime"), isEuroTime(euroTime)
    {
        buildInterface();
    }

    void DateTimeWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DateTimeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_settings_date_and_time"));

        // create date widgets
        uint32_t w = this->getWidth();

        utils::time::Timestamp time;

        // create date widgets
        dateBody = new gui::HBox(this,
                                 style::window::default_left_margin,
                                 style::settings::date::date_box_y_pos,
                                 w,
                                 style::settings::date::date_time_box_h);

        auto item = addDateTimeItem(nullptr,
                                    utils::translate("app_settings_title_day"),
                                    time.get_date_time_substr(utils::time::GetParameters::Day));
        dateItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Day, item));

        item = addDateTimeItem(nullptr,
                               utils::translate("app_settings_title_month"),
                               time.get_date_time_substr(utils::time::GetParameters::Month));
        dateItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Month, item));

        item = addDateTimeItem(nullptr,
                               utils::translate("app_settings_title_year"),
                               time.get_date_time_substr(utils::time::GetParameters::Year));
        dateItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Year, item));

        if (isEuroTime) {
            auto toAdd = dateItems.find(DateTimeItems::Day);
            if (toAdd != dateItems.end()) {
                dateBody->addWidget(toAdd->second);
                dateBody->addWidget(addSpacer(""));
            }
            toAdd = dateItems.find(DateTimeItems::Month);
            if (toAdd != dateItems.end()) {
                dateBody->addWidget(toAdd->second);
            }
        }
        else {
            auto toAdd = dateItems.find(DateTimeItems::Month);
            if (toAdd != dateItems.end()) {
                dateBody->addWidget(toAdd->second);
                dateBody->addWidget(addSpacer(""));
            }
            toAdd = dateItems.find(DateTimeItems::Day);
            if (toAdd != dateItems.end()) {
                dateBody->addWidget(toAdd->second);
            }
        }
        dateBody->addWidget(addSpacer(""));

        auto toAdd = dateItems.find(DateTimeItems::Year);
        if (toAdd != dateItems.end()) {
            dateBody->addWidget(toAdd->second);
        }

        dateBody->setEdges(gui::RectangleEdge::None);

        // create time items

        auto hourValue = time.get_date_time_sub_value(utils::time::GetParameters::Hour);

        if (utils::dateAndTimeSettings.isTimeFormat12()) {
            if (hourValue > 12) {
                hourValue -= 12;
                dayPeriod = true;
            }
        }

        timeBody = new gui::HBox(this,
                                 style::window::default_left_margin,
                                 style::settings::date::time_box_y_pos,
                                 w,
                                 style::settings::date::date_time_box_h);
        item     = addDateTimeItem(nullptr, utils::translate("app_settings_title_time"), std::to_string(hourValue));
        timeItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Hour, item));
        timeBody->addWidget(item);
        timeBody->addWidget(addSpacer(":"));

        item = addDateTimeItem(nullptr, (""), time.get_date_time_substr(utils::time::GetParameters::Minute));
        timeItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Minute, item));
        timeBody->addWidget(item);
        timeBody->addWidget(addSpacer(""));

        item = addDateTimeItem(nullptr, (""), (""));
        if (utils::dateAndTimeSettings.isTimeFormat12()) {
            if (dayPeriod) {
                item->setText("PM");
            }
            else {
                item->setText("AM");
            }
        }
        else {
            item->setVisible(false);
        }
        timeItems.insert(std::pair<DateTimeItems, Item *>(DateTimeItems::Hour, item));
        timeBody->addWidget(item);

        timeBody->setEdges(gui::RectangleEdge::None);

        setFocusItem(timeBody);
        timeBody->setNavigationItem(NavigationDirection::UP, dateBody);

        dateBody->setNavigationItem(NavigationDirection::DOWN, timeBody);
    }

    void DateTimeWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void DateTimeWindow::invalidate() noexcept
    {
        dateBody = nullptr;
        timeBody = nullptr;
        dateItems.clear();
        timeItems.clear();
    }

    void DateTimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    gui::Label *DateTimeWindow::addSpacer(const UTF8 &text)
    {
        auto label = new gui::Label(
            nullptr, 0, 0, style::settings::date::date_time_spacer_width, style::settings::date::date_time_item_height);
        label->setEdges(gui::RectangleEdge::None);
        label->setText(text);
        label->setFont(style::window::font::largelight);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        label->activeItem = false;
        label->setVisible(true);
        return label;
    }

    gui::Label *DateTimeWindow::addDateTimeItem(Item *parent, const UTF8 &itemTitle, const UTF8 &value)
    {

        gui::Label *label = new gui::Label(this,
                                           style::settings::date::date_time_x_offset,
                                           0,
                                           style::settings::date::date_time_item_width,
                                           style::settings::date::date_time_item_height);
        label->setMargins(gui::Margins(0, 0, 0, 0));
        label->setFilled(false);
        label->setPenFocusWidth(3);
        label->setPenWidth(1);
        label->setFont(style::window::font::largelight);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        label->setEdges(gui::RectangleEdge::Bottom);
        label->activatedCallback = [=](gui::Item &) {
            this->setRTC();
            return true;
        };
        label->setText(value);

        Label *title = new gui::Label(label,
                                      0,
                                      0,
                                      style::settings::date::date_time_item_width,
                                      style::settings::date::date_time_item_title_height);
        title->setText(itemTitle);
        title->setEdges(gui::RectangleEdge::None);
        title->setFont(style::window::font::verysmall);
        return label;
    }

    bool DateTimeWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = false;
        if (AppWindow::onInput(inputEvent)) {

            return true;
        }
        else {
            if (inputEvent.isShortRelease()) {
                // handle numeric keys
                if (inputEvent.isDigit()) {
                    if (focusItem == dateBody) {
                        setDate(inputEvent.numericValue());
                    }
                    else if (focusItem == timeBody) {
                        setTime(inputEvent.numericValue());
                    }
                    ret = true;
                }
                else if (inputEvent.is(gui::KeyCode::KEY_PND)) {
                    if (focusItem == dateBody || focusItem == timeBody) {
                        auto item = dynamic_cast<gui::Label *>(focusItem->focusItem);
                        if (item != nullptr) {
                            auto itemValue = item->getText();
                            auto len       = itemValue.length();
                            if (len > 0) {
                                itemValue.removeChar(len - 1);
                                if (itemValue.length() == 0) {
                                    itemValue = "0";
                                }
                                item->setText(itemValue);
                                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                            }
                        }
                    }
                    ret = true;
                }
            }
        }
        return ret;
    }
    std::string DateTimeWindow::getDateTimeItemValue(DateTimeItems itemType)
    {

        std::string ret;

        if (itemType == DateTimeItems::Day || itemType == DateTimeItems::Month || itemType == DateTimeItems::Year) {
            auto selected = dateItems.find(itemType);
            if (selected != dateItems.end()) {
                auto item = dynamic_cast<gui::Label *>(selected->second);
                if (item != nullptr) {
                    ret = item->getText();
                }
            }
        }
        if (itemType == DateTimeItems::Hour || itemType == DateTimeItems::Minute) {
            auto selected = timeItems.find(itemType);
            if (selected != timeItems.end()) {
                auto item = dynamic_cast<gui::Label *>(selected->second);
                if (item != nullptr) {
                    ret = item->getText();
                }
            }
        }
        return ret;
    }

    bool DateTimeWindow::setDate(int32_t keyValue)
    {
        auto item = dynamic_cast<gui::Label *>(focusItem->focusItem);

        if (item != nullptr) {
            auto itemValue = item->getText();
            auto key       = std::to_string(keyValue);
            if (itemValue == "0") {
                itemValue = key;
            }
            else {
                itemValue += key;
            }
            item->setText(itemValue);

            if (utils::time::validateDate(getDateTimeItemValue(DateTimeItems::Day),
                                          getDateTimeItemValue(DateTimeItems::Month),
                                          getDateTimeItemValue(DateTimeItems::Year))) {
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            else {
                item->setText(key);
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            return true;
        }
        return false;
    }
    bool DateTimeWindow::setTime(int32_t keyValue)
    {
        auto item = dynamic_cast<gui::Label *>(focusItem->focusItem);

        if (item != nullptr) {
            auto itemValue = item->getText();
            auto key       = std::to_string(keyValue);
            if (itemValue == "0") {
                itemValue = key;
            }
            else {
                itemValue += key;
            }
            item->setText(itemValue);

            if (utils::time::validateTime(getDateTimeItemValue(DateTimeItems::Hour),
                                          getDateTimeItemValue(DateTimeItems::Minute),
                                          utils::dateAndTimeSettings.isTimeFormat12())) {
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            else {
                item->setText(key);
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            return true;
        }
        return false;
    }

    void DateTimeWindow::setRTC(void)
    {
        struct tm timeinfo;
        memset(&timeinfo, 0, sizeof(timeinfo));

        try {
            timeinfo.tm_year = std::stoi(getDateTimeItemValue(DateTimeItems::Year)) - 1900;
            timeinfo.tm_mon  = std::stoi(getDateTimeItemValue(DateTimeItems::Month)) - 1;
            timeinfo.tm_mday = std::stoi(getDateTimeItemValue(DateTimeItems::Day));

            auto hourValue = std::stoi(getDateTimeItemValue(DateTimeItems::Hour));
            if (utils::dateAndTimeSettings.isTimeFormat12()) {
                if (dayPeriod) {
                    hourValue += 12;
                }
            }
            timeinfo.tm_hour = hourValue;
            timeinfo.tm_min  = std::stoi(getDateTimeItemValue(DateTimeItems::Minute));
        }
        catch (std::exception &e) {
            LOG_ERROR("DateTimeWindow::setRTC: %s", e.what());
        }

        bsp::rtc::setDateTime(&timeinfo);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void DateTimeWindow::setEuroTime(bool euroTime)
    {
        isEuroTime = euroTime;
    }

} /* namespace gui */
