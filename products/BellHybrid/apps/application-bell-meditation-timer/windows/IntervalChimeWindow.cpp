// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeWindow.hpp"
#include "ReadyGoingWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(app::Application *app)
        : MeditationWindow(app, gui::name::window::interval_chime)
    {
        buildInterface();
    }

    void IntervalChimeWindow::rebuild()
    {}

    void IntervalChimeWindow::buildInterface()
    {
        MeditationWindow::buildInterface();

        // bottomBar->setActive(BottomBar::Side::CENTER, true);
        // bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        // bottomBar->setActive(BottomBar::Side::RIGHT, true);
        // bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        // setTitle(gui::name::window::interval_chime);

        previousImage = new gui::Image(
            this, icStyle::left_arrow::x, icStyle::left_arrow::y, 0, 0, icStyle::left_arrow::imageSource);
        nextImage = new gui::Image(
            this, icStyle::right_arrow::x, icStyle::right_arrow::y, 0, 0, icStyle::right_arrow::imageSource);

        auto newLabel = [](Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
            auto label = new gui::Label(parent, x, y, w, h);
            label->setFilled(false);
            label->setBorderColor(gui::ColorNoColor);
            label->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            return label;
        };

        title = newLabel(this, icStyle::title::x, icStyle::title::y, icStyle::title::w, icStyle::title::h);
        title->setFont(style::window::font::medium);
        title->setText(gui::name::window::interval_chime);

        text = newLabel(this, icStyle::text::x, icStyle::text::y, icStyle::text::w, icStyle::text::h);
        text->setFont(style::window::font::supersizemelight);

        minute = newLabel(this, icStyle::minute::x, icStyle::minute::y, icStyle::minute::w, icStyle::minute::h);
        minute->setFont(style::window::font::small);
        minute->setText("Minutes");
    }

    void IntervalChimeWindow::destroyInterface()
    {
        erase();
        previousImage = nullptr;
        nextImage     = nullptr;
        text          = nullptr;
        minute        = nullptr;
    }

    void IntervalChimeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_DEBUG("onBeforeShow: mode=%s", mode == ShowMode::GUI_SHOW_INIT ? "GUI_SHOW_INIT" : "GUI_SHOW_RETURN");

        MeditationWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            updateDisplay();
        }
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT) || inputEvent.isShortRelease(gui::KeyCode::KEY_UP)) {
            previousInterval();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RIGHT) || inputEvent.isShortRelease(gui::KeyCode::KEY_DOWN)) {
            nextInterval();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::ready_going);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    void IntervalChimeWindow::previousInterval()
    {
        LOG_DEBUG("previous");

        if (intervalType > INTERVAL_NONE) {
            intervalType--;
        }
        else {
            intervalType = INTERVAL_15;
        }
        item.setIntervalSecs(intervalToSecs());
        updateDisplay();
    }

    void IntervalChimeWindow::nextInterval()
    {
        LOG_DEBUG("next");

        if (intervalType < INTERVAL_15) {
            intervalType++;
        }
        else {
            intervalType = INTERVAL_NONE;
        }
        item.setIntervalSecs(intervalToSecs());
        updateDisplay();
    }

    void IntervalChimeWindow::updateDisplay()
    {
        text->setText(getIntervalString());
    }

    std::string IntervalChimeWindow::getIntervalString()
    {
        switch (intervalType) {
        case INTERVAL_NONE:
            return "None";
        case INTERVAL_1:
            return "every 1";
        case INTERVAL_2:
            return "every 2";
        case INTERVAL_5:
            return "every 5";
        case INTERVAL_10:
            return "every 10";
        case INTERVAL_15:
            return "every 15";
        }
        return "None";
    }

    uint32_t IntervalChimeWindow::intervalToSecs()
    {
        switch (intervalType) {
        case INTERVAL_NONE:
            return 0;
        case INTERVAL_1:
            return 60;
        case INTERVAL_2:
            return 2 * 60;
        case INTERVAL_5:
            return 5 * 60;
        case INTERVAL_10:
            return 10 * 60;
        case INTERVAL_15:
            return 15 * 60;
        }
        return 0;
    }
} // namespace gui
