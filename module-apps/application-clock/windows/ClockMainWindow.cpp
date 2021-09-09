// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// module-gui
#include "application-clock/windows/ClockMainWindow.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/ProgressBar.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/core/ImageManager.hpp"
#include "../ClockData.hpp"
#include <Style.hpp>

namespace gui
{

    ClockMainWindow::ClockMainWindow(app::ApplicationCommon *app, const std::string &name) : gui::AppWindow(app, name)
    {
        uint32_t xOffset = 0;
        uint32_t yOffset = 0;

        hourLabel = new gui::Label(this, 100 + xOffset, 300 - 160 + yOffset, 280, 150);
        hourLabel->setFilled(false);
        hourLabel->setBorderColor(gui::ColorNoColor);
        hourLabel->setFont(style::window::font::largelight);
        hourLabel->setText("00");
        hourLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        minuteLabel = new gui::Label(this, 100 + xOffset, 310 + yOffset, 280, 150);
        minuteLabel->setFilled(false);
        minuteLabel->setBorderColor(gui::ColorFullBlack);
        minuteLabel->setFont(style::window::font::largelight);
        minuteLabel->setText("00");
        minuteLabel->setRadius(20);
        minuteLabel->setPenWidth(1);
        minuteLabel->setFillColor(gui::Color(10, 0));
        minuteLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        uint32_t x = 480 / 2 - 90 + xOffset, y = 300 - 6 + yOffset, w = 180, h = 12;
        progressBar = new ProgressBar(this, x, y, w, h);
        progressBar->setMaximum(59);
        progressBar->setValue(0);
    }

    bool ClockMainWindow::handleSwitchData(SwitchData *data)
    {
        ClockData *clockData = reinterpret_cast<ClockData *>(data);

        if (clockData) {
            hour   = clockData->hour % 24;
            minute = clockData->minute % 60;
        }

        return true;
    }

    void ClockMainWindow::updateLabels()
    {
        std::string h;
        if (hour < 10) {
            h += "0";
        }
        h += std::to_string(hour);
        hourLabel->setText(h);

        std::string m;
        if (minute < 10) {
            m += "0";
        }
        m += std::to_string(minute);
        minuteLabel->setText(m);
    }

    bool ClockMainWindow::incrementHour()
    {
        hour++;
        if (hour > 23) {
            hour = 0;
            return true;
        }
        return false;
    }
    bool ClockMainWindow::incrementMinute()
    {
        minute++;
        if (minute > 59) {
            incrementHour();
            minute = 0;
            return true;
        }
        return false;
    }

    bool ClockMainWindow::incrementSecond()
    {
        bool ret = false;
        seconds++;
        if (seconds > 59) {
            incrementMinute();
            seconds = 0;
            ret     = true;
        }
        progressBar->setValue(seconds);

        return ret;
    }

} /* namespace gui */
