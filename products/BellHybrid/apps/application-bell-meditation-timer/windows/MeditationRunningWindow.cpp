// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationRunningWindow.hpp"
#include "SessionEndWindow.hpp"
#include "SessionPausedWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/ServiceTime.hpp>

#include <log/log.hpp>
#include <i18n/i18n.hpp>

namespace
{
    inline constexpr auto meditationProgressTimerName = "MeditationProgressTimer";
    inline constexpr std::chrono::seconds baseTick{1};
    inline constexpr auto meditationProgressMode = app::ProgressCountdownMode::Decreasing;

    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }

    gui::Label *createDatetime(gui::Item *parent)
    {
        auto datetime = new gui::Label(parent, 0, 0, parent->getWidth(), parent->getHeight());
        datetime->setFont(mrStyle::datetime::font);
        decorateProgressItem(datetime, gui::Alignment::Vertical::Center);
        return datetime;
    }

    gui::Label *createTitle(gui::VBox *parent)
    {
        auto title = new gui::Label(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        title->setText(utils::translate("app_meditation_title_main"));
        title->setFont(mrStyle::title::font);
        decorateProgressItem(title, gui::Alignment::Vertical::Top);
        return title;
    }

    gui::UnityProgressBar *createProgress(gui::VBox *parent)
    {
        auto progress = new gui::UnityProgressBar(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        progress->setDisplaySize(mrStyle::progress::w, mrStyle::progress::h);
        progress->setMaximum(100);
        decorateProgressItem(progress, gui::Alignment::Vertical::Center);
        return progress;
    }

    gui::Text *createTimer(gui::Item *parent)
    {
        auto timer = new gui::Text(parent, 0, 0, style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        timer->setFont(mrStyle::timer::font);
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
    }
} // namespace

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

        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);

        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        datetime = createDatetime(body->firstBox);
        createTitle(vBox);
        progress = createProgress(vBox);
        timer    = createTimer(body->lastBox);
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
        presenter->get(item);
    }

    void MeditationRunningWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item != nullptr) {
            presenter->set(*item);
        }
        updateDateTime();
    }

    void MeditationRunningWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimer>(
            application, *this, meditationProgressTimerName, baseTick, meditationProgressMode);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
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
