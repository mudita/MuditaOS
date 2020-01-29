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

#include "i18/i18.hpp"
#include "time/time_conversion.hpp"

#include "DateTimeWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include <Style.hpp>

namespace gui
{

    DateTimeWindow::DateTimeWindow(app::Application *app) : AppWindow(app, "DateTime")
    {
        setSize(480, 600);

        buildInterface();
    }

    void DateTimeWindow::rebuild()
    {
        // find which widget has focus
        uint32_t index = 0;

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
        // todo add date format to settings db, false american layout
        bool dateFormat = false;
        utils::time::Time time;

        if (dateFormat)
        {
            dateItems.push_back(addDateTimeItem(utils::localize.get("app_settings_title_day"), time.getMDay(), [=](gui::Item &) {
                LOG_INFO("Activated callback");
                return true;
            }));

            dateItems.push_back(addDateTimeItem(utils::localize.get("app_settings_title_month"), time.getMonth(), [=](gui::Item &) {
                LOG_INFO("Activated callback");
                return true;
            }));
        }
        else
        {
            dateItems.push_back(addDateTimeItem(utils::localize.get("app_settings_title_month"), time.getMonth(), [=](gui::Item &) {
                LOG_INFO("Activated callback");
                return true;
            }));

            dateItems.push_back(addDateTimeItem(utils::localize.get("app_settings_title_day"), time.getMDay(), [=](gui::Item &) {
                LOG_INFO("Activated callback");
                return true;
            }));
        }

        dateItems.push_back(addDateTimeItem(utils::localize.get("app_settings_title_year"), time.getYear(), [=](gui::Item &) {
            LOG_INFO("Activated callback");
            return true;
        }));

        for (uint32_t i = 0; i < dateItems.size(); i++)
        {
            uint32_t y = 228;
            uint32_t x = 30 + 150 * i;
            dateItems[i]->setPosition(x, y);
        }
    }
    void DateTimeWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        this->focusItem = nullptr;
        children.clear();
    }

    DateTimeWindow::~DateTimeWindow()
    {
    }

    void DateTimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        //	setFocusItem( options[0] );
    }

    gui::Item *DateTimeWindow::addDateTimeItem(const UTF8 &itemTitle, const UTF8 &value, std::function<bool(Item &)> activatedCallback)
    {
        gui::Label *label = new gui::Label(this, 30, 228, 120, 107);
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
} /* namespace gui */
