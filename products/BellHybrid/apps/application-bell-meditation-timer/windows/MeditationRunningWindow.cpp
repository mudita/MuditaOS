// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationRunningWindow.hpp"
#include "SessionPausedWindow.hpp"
#include "SessionEndWindow.hpp"
#include "../data/MeditationSwitchData.hpp"

#include <service-time/ServiceTime.hpp>

#include "log.hpp"
#include <i18n/i18n.hpp>
#include <Font.hpp>
#include <TextBlock.hpp>

namespace gui
{
    MeditationRunningWindow::MeditationRunningWindow(app::Application *app)
        : WithTimerWindow(app, gui::name::window::meditation_running)
    {
        buildInterface();
    }

    void MeditationRunningWindow::buildInterface()
    {
        WithTimerWindow::buildInterface();

        auto newLabel = [](Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
            auto label = new gui::Label(parent, x, y, w, h);
            label->setFilled(false);
            label->setBorderColor(gui::ColorNoColor);
            label->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            return label;
        };

        datetime =
            newLabel(this, mrStyle::datetime::x, mrStyle::datetime::y, mrStyle::datetime::w, mrStyle::datetime::h);
        datetime->setFont(mrStyle::datetime::font);

        title = newLabel(this, mrStyle::title::x, mrStyle::title::y, mrStyle::title::w, mrStyle::title::h);
        title->setText(utils::translate("app_meditation_title_main"));
        title->setFont(mrStyle::title::font);

        timer = newLabel(this, mrStyle::timer::x, mrStyle::timer::y, mrStyle::timer::w, mrStyle::timer::h);
        timer->setFont(mrStyle::timer::font);

        progress = new UnityProgressBar(
            this, mrStyle::progress::x, mrStyle::progress::y, mrStyle::progress::w, mrStyle::progress::h);
    }

    void MeditationRunningWindow::destroyInterface()
    {
        erase();
        datetime = nullptr;
        title    = nullptr;
        timer    = nullptr;
        progress = nullptr;
    }

    void MeditationRunningWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WithTimerWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            start();
        }
        else {
            resume();
        }
    }

    bool MeditationRunningWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            pause();
            return true;
        }
        if (inputEvent.is(gui::KeyCode::KEY_RF)) {
            return true;
        }

        return WithTimerWindow::onInput(inputEvent);
    }

    void MeditationRunningWindow::onTimeout()
    {
        LOG_DEBUG("onTimeout");
        passedTimer += std::chrono::seconds{1};
        passedInterval += std::chrono::seconds{1};

        if (passedInterval == item.getInterval()) {
            passedInterval = std::chrono::seconds::zero();
            intervalTimeout();
        }
        if (passedTimer == item.getTimer()) {
            endSession();
        }
        else {
            updateProgress();
            updateDateTime();
            updateTimer();
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
    }

    void MeditationRunningWindow::updateDateTime()
    {
        using namespace utils::time;
        auto timestamp = utils::time::getCurrentTimestamp();
        if (datetime != nullptr) {
            auto fmt = Locale::format(stm::api::timeFormat());
            /*utils::dateAndTimeSettings.isTimeFormat12()
                        ? Locale::format(Locale::TimeFormat::FormatTime12H)
                        : Locale::format(Locale::TimeFormat::FormatTime24H);*/
            datetime->setText(timestamp.str(fmt));
        }
    }

    void MeditationRunningWindow::updateTimer()
    {
        uint32_t remained = item.getTimer().count() - passedTimer.count();
        uint32_t minutes  = remained / 60;
        uint32_t seconds  = remained % 60;
        char buffer[32];

        sprintf(buffer, "%ld:%02ld", minutes, seconds);
        timer->setText(std::string(buffer));
    }

    void MeditationRunningWindow::updateProgress()
    {
        uint32_t percent = std::lround(passedTimer.count() * 100 / item.getTimer().count());

        progress->setPercentageValue(percent);
    }

    void MeditationRunningWindow::start()
    {
        LOG_DEBUG("start");
        progress->setMaximum(item.getTimer().count());
        progress->setValue(0);
        updateDateTime();
        updateTimer();
        startTimer(std::chrono::seconds{1}, true);
        playGong();
    }

    void MeditationRunningWindow::pause()
    {
        LOG_DEBUG("pause");

        stopTimer();
        gotoWindow(gui::name::window::session_paused);
    }

    void MeditationRunningWindow::resume()
    {
        LOG_DEBUG("resume");

        updateProgress();
        updateDateTime();
        updateTimer();
        startTimer(std::chrono::seconds{1}, true);
    }

    void MeditationRunningWindow::intervalTimeout()
    {
        LOG_DEBUG("intervalTimeout");
        playGong();
    }

    void MeditationRunningWindow::endSession()
    {
        LOG_DEBUG("endSession");

        playGong();
        stopTimer();
        gotoWindow(gui::name::window::session_end);
    }

    void MeditationRunningWindow::playGong()
    {
        LOG_DEBUG("playGong");
        // AudioServiceAPI::PlaybackStart(application,
        //                               audio::PlaybackType::Meditation,
        //                               purefs::dir::getCurrentOSPath() / "assets/audio/meditation/gong.mp3");
    }
} // namespace gui
