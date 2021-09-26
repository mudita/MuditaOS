// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationRunningWindow.hpp"
#include "SessionPausedWindow.hpp"
#include "SessionEndWindow.hpp"

#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/ServiceTime.hpp>

#include <log/log.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    MeditationRunningWindow::MeditationRunningWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter)
        : MeditationWindow(app, gui::name::window::meditation_running), presenter{std::move(windowPresenter)}
    {
        buildInterface();
        configureTimer();
        presenter->attach(this);
    }

    void MeditationRunningWindow::buildInterface()
    {
        MeditationWindow::buildInterface();

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

        timer = new gui::Text(this, mrStyle::timer::x, mrStyle::timer::y, mrStyle::timer::w, mrStyle::timer::h);
        timer->setFont(mrStyle::timer::font);
        timer->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        progress = new gui::UnityProgressBar(
            this, mrStyle::progress::x, mrStyle::progress::y, mrStyle::progress::w, mrStyle::progress::h);
        progress->setMaximum(100);
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
        MeditationWindow::onBeforeShow(mode, data);

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

        return MeditationWindow::onInput(inputEvent);
    }

    void MeditationRunningWindow::pregressFinished()
    {
        endSession();
    }

    void MeditationRunningWindow::intervalReached()
    {
        intervalTimeout();
    }

    void MeditationRunningWindow::baseTickReached()
    {
        updateDateTime();
    }

    void MeditationRunningWindow::buildMeditationItem(MeditationItem &item)
    {
        presenter->request(item);
    }

    void MeditationRunningWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item != nullptr) {
            presenter->activate(*item);
        }
        updateDateTime();
    }

    void MeditationRunningWindow::configureTimer()
    {
        presenter->initTimer(this);
        presenter->getUIConfigurator().attach(progress);
        presenter->getUIConfigurator().attach(timer);
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

    void MeditationRunningWindow::start()
    {
        LOG_DEBUG("start");
        presenter->start();
        playGong();
    }

    void MeditationRunningWindow::pause()
    {
        LOG_DEBUG("pause");
        presenter->pause();
        gotoWindow(gui::name::window::session_paused);
    }

    void MeditationRunningWindow::resume()
    {
        LOG_DEBUG("resume");
        presenter->resume();
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
        gotoWindow(gui::name::window::session_end);
    }

    void MeditationRunningWindow::playGong()
    {
        LOG_DEBUG("playGong");
        AudioServiceAPI::PlaybackStart(application,
                                       audio::PlaybackType::Meditation,
                                       purefs::dir::getCurrentOSPath() / "assets/audio/meditation/gong.mp3");
    }
} // namespace gui
