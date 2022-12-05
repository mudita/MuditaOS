// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningLoopWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <data/RelaxationSwitchData.hpp>

#include <ApplicationBellRelaxation.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <Units.hpp>

namespace
{
    inline constexpr auto relaxationTimerName = "RelaxationLoopTimer";
    inline constexpr std::chrono::seconds timerTick{1};
    inline constexpr units::SOC dischargingLevelShowTop = 20;
    inline constexpr units::SOC dischargingLowBattery   = 10;

    bool isBatteryCharging(const Store::Battery::State state)
    {
        using State = Store::Battery::State;
        return state == State::Charging or state == State::ChargingDone;
    }

    gui::Text *createTitle(gui::VBox *parent)
    {
        namespace relaxationStyle = gui::relaxationStyle;
        auto title                = new gui::TextFixedSize(parent);
        title->setFont(relaxationStyle::titleFont);
        title->setMinimumWidth(relaxationStyle::title::width);
        title->setMinimumHeightToFitText(relaxationStyle::title::maxLines);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->drawUnderline(false);
        return title;
    }

    gui::BellStatusClock *createClock(gui::Item *parent)
    {
        auto time = new gui::BellStatusClock(parent);
        time->setFont(gui::relaxationStyle::clockFont);
        time->setMaximumSize(parent->getWidth(), parent->getHeight());
        time->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        return time;
    }

    gui::Text *createBottomText(gui::VBox *parent)
    {
        namespace relaxationStyle = gui::relaxationStyle;
        auto title                = new gui::TextFixedSize(parent);
        title->setFont(relaxationStyle::descriptionFont);
        title->setMinimumWidth(relaxationStyle::text::minWidth);
        title->setMinimumHeightToFitText(relaxationStyle::text::maxLines);
        title->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        title->drawUnderline(false);
        title->setText(utils::translate("app_bell_relaxation_looped"));
        title->setVisible(false);
        return title;
    }

    gui::BellBattery *createBattery(gui::VBox *parent)
    {
        auto battery = new gui::BellBattery(parent, gui::BatteryWidthMode::Fixed);
        battery->setMinimumSize(gui::battery::battery_widget_w, gui::battery::battery_widget_h);
        battery->setEdges(gui::RectangleEdge::None);
        battery->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        battery->setBatteryPercentMode(gui::BatteryPercentMode::Show);
        battery->setVisible(false);
        return battery;
    }
} // namespace

namespace gui
{
    RelaxationRunningLoopWindow::RelaxationRunningLoopWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::relaxation::RelaxationRunningLoopContract::Presenter> &&presenter)
        : AppWindow(app, gui::window::name::relaxationRunningLoop), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void RelaxationRunningLoopWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateBatteryStatus();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_RETURN && presenter->isPaused()) {
            presenter->resume();
            return;
        }

        if (data && typeid(*data) == typeid(RelaxationSwitchData)) {
            const auto battery    = presenter->handleBatteryStatus();
            auto *audioSwitchData = static_cast<RelaxationSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
            title->setText(audioContext->getSound().tags.title);
            if (battery.level > dischargingLowBattery) {
                presenter->activate(audioContext->getSound());
            }
        }
    }

    void RelaxationRunningLoopWindow::buildInterface()
    {
        AppWindow::buildInterface();
        configureTimer();
        buildLayout();
    }

    void RelaxationRunningLoopWindow::buildLayout()
    {
        using Alignment = gui::Alignment;
        statusBar->setVisible(false);
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);

        title      = createTitle(body->centerBox);
        time       = createClock(body->firstBox);
        battery    = createBattery(body->lastBox);
        bottomText = createBottomText(body->lastBox);

        body->lastBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        updateBatteryStatus();
        body->resizeItems();

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void RelaxationRunningLoopWindow::configureTimer()
    {
        auto timer = std::make_unique<app::ProgressTimer>(
            application, *this, relaxationTimerName, timerTick, app::ProgressCountdownMode::Increasing);
        presenter->setTimer(std::move(timer));
    }

    bool RelaxationRunningLoopWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                presenter->stop();
                application->returnToPreviousWindow();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER)) {
                presenter->pause();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN) || inputEvent.is(KeyCode::KEY_UP)) {
                application->switchWindow(gui::popup::window::volume_window);
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void RelaxationRunningLoopWindow::onFinished()
    {
        application->returnToPreviousWindow();
    }

    void RelaxationRunningLoopWindow::onPaused()
    {
        application->switchWindow(gui::window::name::relaxationPaused);
    }

    void RelaxationRunningLoopWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(true);
    }

    void RelaxationRunningLoopWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    void RelaxationRunningLoopWindow::resume()
    {}

    RefreshModes RelaxationRunningLoopWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }

    bool RelaxationRunningLoopWindow::updateBatteryStatus()
    {
        auto batteryStatus = presenter->handleBatteryStatus();

        units::SOC soc = batteryStatus.level;
        auto state     = batteryStatus.state;

        checkLowBatteryLevel(soc);

        if (soc < dischargingLevelShowTop) {
            battery->update(soc, isBatteryCharging(state));
            bottomText->setVisible(false);
            battery->setVisible(true);
            battery->informContentChanged();
        }
        else {
            battery->setVisible(false);
            bottomText->setVisible(true);
            bottomText->informContentChanged();
        }

        return true;
    }

    void RelaxationRunningLoopWindow::checkLowBatteryLevel(units::SOC soc)
    {
        if (soc <= dischargingLowBattery) {
            presenter->stop();
            application->switchWindow(gui::window::name::relaxationLowBattery,
                                      std::make_unique<gui::RelaxationLowBatterySwitchData>(soc));
        }
    }
} // namespace gui
