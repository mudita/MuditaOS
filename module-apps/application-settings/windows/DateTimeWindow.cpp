/*
 * DateTimeWindow.cpp
 *
 *  Created on: 28 sty 2020
 *      Author: kuba
 */

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"
#include "DateTimeWindow.hpp"

#include "i18/i18.hpp"
#include "time/time_conversion.hpp"
#include "time/time_date_validation.hpp"

#include "../widgets/DateTimeItem.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "Text.hpp"
#include <Style.hpp>
#include <widgets/BoxLayout.hpp>

namespace gui
{

    DateTimeWindow::DateTimeWindow(app::Application *app) : AppWindow(app, "DateTime")
    {
        setSize(480, 600);

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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_settings_date_and_time"));

        // create date widgets
        uint32_t w = this->getWidth() - style::window::default_left_margin * 2;
        uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();
        utils::time::SysTime time;
        auto str = time.str(time.today_format);

        SettingsRecord appSettings = application->getSettings();
        timeFormat12h = appSettings.timeFormat12;
        timeDateFormat = appSettings.timeDateFormat;

        // create date widgets
        dateBody = new gui::HBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);

        auto item = addDateTimeItem(nullptr, utils::localize.get("app_settings_title_day"), time.get_date_time_substr(utils::time::GetParameters::Day),
                                    [=](gui::Item &) {
                                        LOG_INFO("Activated callback");
                                        return true;
                                    });
        dateItems.insert(std::pair<DateItems, Item *>(DateItems::Day, item));

        item = addDateTimeItem(nullptr, utils::localize.get("app_settings_title_month"), time.get_date_time_substr(utils::time::GetParameters::Month),
                               [=](gui::Item &) {
                                   LOG_INFO("Activated callback");
                                   return true;
                               });
        dateItems.insert(std::pair<DateItems, Item *>(DateItems::Month, item));

        item = addDateTimeItem(nullptr, utils::localize.get("app_settings_title_year"), time.get_date_time_substr(utils::time::GetParameters::Year),
                               [=](gui::Item &) {
                                   LOG_INFO("Activated callback");
                                   return true;
                               });
        dateItems.insert(std::pair<DateItems, Item *>(DateItems::Year, item));

        if (timeDateFormat)
        {
            auto toAdd = dateItems.find(DateItems::Day);
            if (toAdd != dateItems.end())
            {
                dateBody->tryAddWidget(toAdd->second);
            }
            toAdd = dateItems.find(DateItems::Month);
            if (toAdd != dateItems.end())
            {
                dateBody->tryAddWidget(toAdd->second);
            }
        }
        else
        {
            auto toAdd = dateItems.find(DateItems::Month);
            if (toAdd != dateItems.end())
            {
                dateBody->tryAddWidget(toAdd->second);
            }
            toAdd = dateItems.find(DateItems::Day);
            if (toAdd != dateItems.end())
            {
                dateBody->tryAddWidget(toAdd->second);
            }
        }
        auto toAdd = dateItems.find(DateItems::Year);
        if (toAdd != dateItems.end())
        {
            dateBody->tryAddWidget(toAdd->second);
        }

        dateBody->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        // create time items

        auto hourValue = time.get_date_time_sub_value(utils::time::GetParameters::Hour);
        // day period indicator false am, true pm
        bool dayPeriod = false;
        if (timeFormat12h)
        {
            if (hourValue > 12)
            {
                hourValue -= 12;
                dayPeriod = true;
            }
        }

        timeBody = new gui::HBox(this, style::window::default_left_margin, (uint32_t)title->offset_h() * 2, w, h);
        item = addDateTimeItem(nullptr, utils::localize.get("app_settings_title_time"), std::to_string(hourValue), [=](gui::Item &) {
            LOG_INFO("Activated callback");
            return true;
        });
        timeItems.insert(std::pair<TimeItems, Item *>(TimeItems::Hour, item));
        timeBody->tryAddWidget(item);

        item = addDateTimeItem(nullptr, (""), time.get_date_time_substr(utils::time::GetParameters::Minute), [=](gui::Item &) {
            LOG_INFO("Activated callback");
            return true;
        });
        timeItems.insert(std::pair<TimeItems, Item *>(TimeItems::Hour, item));
        timeBody->tryAddWidget(item);

        item = addDateTimeItem(nullptr, (""), (""), [=](gui::Item &) {
            LOG_INFO("Activated callback");
            return true;
        });
        if (timeFormat12h)
        {
            if (dayPeriod)
            {
                item->setText("PM");
            }
            else
            {
                item->setText("AM");
            }
        }
        else
        {
            item->setVisible(false);
        }
        timeItems.insert(std::pair<TimeItems, Item *>(TimeItems::Hour, item));
        timeBody->tryAddWidget(item);

        setFocusItem(timeBody);
        timeBody->setNavigationItem(NavigationDirection::UP, dateBody);
        dateBody->setNavigationItem(NavigationDirection::DOWN, timeBody);
    }
    void DateTimeWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        this->focusItem = nullptr;

        //        for (uint32_t i = 0; i < dateItems.size(); i++)
        //        {
        //            delete dateItems[i];
        //        }

        dateItems.clear();
        timeItems.clear();
        children.clear();
    }

    DateTimeWindow::~DateTimeWindow()
    {
    }

    void DateTimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        //	setFocusItem( options[0] );
    }

    gui::Label *DateTimeWindow::addDateTimeItem(Item *parent, const UTF8 &itemTitle, const UTF8 &value, std::function<bool(Item &)> activatedCallback)
    {
        gui::Label *label = new gui::Label(this, 30, 0, 120 + 0, 107);
        label->setMargins(gui::Margins(16, 0, 0, 0));
        label->setFilled(false);
        label->setPenFocusWidth(3);
        label->setPenWidth(1);
        label->setFont(style::window::font::verybig);
        label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        label->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        label->activatedCallback = activatedCallback;
        label->setText(value);

        Label *title = new gui::Label(label, 0, 0, 120, 30);
        title->setText(itemTitle);
        title->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        title->setFont(style::window::font::verysmall);
        return label;
    }

    bool DateTimeWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = false;
        if (AppWindow::onInput(inputEvent))
        {
            return true;
        }
        else
        {
            if (inputEvent.state == gui::InputEvent::State::keyReleasedShort)
            {
                auto key = std::to_string(gui::toNumeric(inputEvent.keyCode));
                LOG_INFO("Pressed: %s", key.c_str());
                auto value = gui::toNumeric(inputEvent.keyCode);
                // handle numeric keys
                if (value >= 0)
                {
                    if (focusItem == dateBody)
                    {
                        setDate(value);
                    }
                    else if (focusItem == timeBody)
                    {
                        setTime(value);
                    }
                }
                else if (inputEvent.keyCode == gui::KeyCode::KEY_AST)
                {
                    if (focusItem == dateBody || focusItem == timeBody)
                    {
                        auto item = dynamic_cast<gui::Label *>(focusItem->focusItem);
                        if (item != nullptr)
                        {
                            auto itemValue = item->getText();
                            auto len = itemValue.length();
                            if (len > 1)
                            {
                                itemValue.removeChar(len - 1);
                                item->setText(itemValue);
                                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }
    std::string DateTimeWindow::getDateItemValue(DateItems itemType)
    {

        std::string ret;
        auto selected = dateItems.find(itemType);
        if (selected != dateItems.end())
        {
            auto item = dynamic_cast<gui::Label *>(selected->second);
            if (item != nullptr)
            {
                ret = item->getText();
            }
        }
        return ret;
    }
    bool DateTimeWindow::setDate(int32_t keyValue)
    {
        auto item = dynamic_cast<gui::Label *>(focusItem->focusItem);

        if (item != nullptr)
        {
            auto itemValue = item->getText();
            auto key = std::to_string(keyValue);
            itemValue += key;
            item->setText(itemValue);

            if (utils::time::validateDate(getDateItemValue(DateItems::Day), getDateItemValue(DateItems::Month), getDateItemValue(DateItems::Year)))
            {
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            else
            {
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

        if (item != nullptr)
        {
            auto itemValue = item->getText();
            auto key = std::to_string(keyValue);
            itemValue += key;
            item->setText(itemValue);

            if (utils::time::validateTime(getItemValue(TimeItems::Hour), getItemValue(TimeItems::Minute), timeFormat12h))
            {
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            else
            {
                item->setText(key);
                application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
            }
            return true;
        }
        return false;
    }
} /* namespace gui */
