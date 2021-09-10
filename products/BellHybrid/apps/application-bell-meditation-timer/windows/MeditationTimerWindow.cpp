// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerWindow.hpp"
#include "IntervalChimeWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include "log.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    MeditationTimerWindow::MeditationTimerWindow(app::Application *app)
        : MeditationWindow(app, gui::name::window::meditation_timer)
    {
        buildInterface();
    }

    void MeditationTimerWindow::rebuild()
    {}

    void MeditationTimerWindow::buildInterface()
    {
        MeditationWindow::buildInterface();
        statusBar->setVisible(false);
        // bottomBar->setActive(BottomBar::Side::CENTER, true);
        // bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        // bottomBar->setActive(BottomBar::Side::RIGHT, true);
        // bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        // setTitle(gui::name::window::meditation_timer);

        auto newLabel = [](Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
            auto label = new gui::Label(parent, x, y, w, h);
            label->setFilled(false);
            label->setBorderColor(gui::ColorNoColor);
            label->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            return label;
        };

        title = newLabel(this, mtStyle::title::x, mtStyle::title::y, mtStyle::title::w, mtStyle::title::h);
        title->setFont(style::window::font::big);
        title->setText(gui::name::window::meditation_timer);

        text = newLabel(this, mtStyle::text::x, mtStyle::text::y, mtStyle::text::w, mtStyle::text::h);
        text->setFont(style::window::font::supersizemelight);

        minute = newLabel(this, mtStyle::minute::x, mtStyle::minute::y, mtStyle::minute::w, mtStyle::minute::h);
        minute->setFont(style::window::font::small);
        minute->setText("Minutes");
    }

    void MeditationTimerWindow::destroyInterface()
    {
        erase();
        text   = nullptr;
        minute = nullptr;
    }

    void MeditationTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            updateDisplay();
        }
    }

    bool MeditationTimerWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_UP) || inputEvent.isShortRelease(gui::KeyCode::KEY_RIGHT)) {
            increaseTimer();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_DOWN) || inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            decreaseTimer();
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            gotoWindow(gui::name::window::interval_chime);
            return true;
        }

        return MeditationWindow::onInput(inputEvent);
    }

    void MeditationTimerWindow::increaseTimer()
    {
        LOG_DEBUG("increaseTimer");

        uint32_t t = item.getTimerSecs();
        if (t >= meditation::value::TIMER_MAX) {
            return;
        }
        item.setTimerSecs(t + 60);
        updateDisplay();
    }

    void MeditationTimerWindow::decreaseTimer()
    {
        LOG_DEBUG("decreaseTimer");

        uint32_t t = item.getTimerSecs();
        if (t <= meditation::value::TIMER_MIN) {
            return;
        }
        item.setTimerSecs(t - 60);
        updateDisplay();
    }

    void MeditationTimerWindow::updateDisplay()
    {
        text->setText(std::to_string(item.getTimerSecs() / 60));
    }
} // namespace gui
