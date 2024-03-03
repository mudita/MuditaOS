// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningLoopWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <data/RelaxationSwitchData.hpp>
#include <popups/data/AudioErrorParams.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <common/widgets/BellBattery.hpp>
#include <common/BellCommonNames.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <ApplicationBellRelaxation.hpp>

namespace
{
    constexpr auto relaxationLoopTimerName{"RelaxationLoopTimer"};
    constexpr std::chrono::seconds relaxationLoopTimerPeriod{1};
    constexpr units::SOC dischargingLevelShowTop{20};

    std::string adjustDisplayedTitle(const UTF8 &title, const std::uint32_t maxCharsInLine)
    {
        /* charsMultiplier is set a little bit less than max lines which is 2, because of final text formatting */
        constexpr auto charsMultiplier{1.8f};
        constexpr auto ellipsisSpace{2U};
        constexpr auto ellipsis{"..."};

        const auto maxTitleLength =
            static_cast<std::uint32_t>(std::round(static_cast<float>(maxCharsInLine) * charsMultiplier));
        if (title.length() <= maxTitleLength) {
            return title;
        }

        auto newTitle = title.substr(0, maxTitleLength - ellipsisSpace);
        newTitle += ellipsis;
        return newTitle;
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
            auto audioSwitchData      = static_cast<RelaxationSwitchData *>(data);
            audioContext              = audioSwitchData->getAudioContext();
            const auto maxCharsInLine = titleText->getTextFormat().getFont()->getCharCountInSpace(
                audioContext->getSound().tags.title, titleText->getWidth());
            titleText->setText(adjustDisplayedTitle(audioContext->getSound().tags.title, maxCharsInLine));
            presenter->activate(audioContext->getSound());
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
        statusBar->setVisible(false);

        auto mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainVBox->setEdges(rectangle_enums::RectangleEdge::None);

        clock = new BellStatusClock(mainVBox);
        clock->setMaximumSize(relaxationStyle::relStyle::clock::maxSizeX, relaxationStyle::relStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(Margins(0, relaxationStyle::relStyle::clock::marginTop, 0, 0));

        titleText = new TextFixedSize(mainVBox);
        titleText->setFont(relaxationStyle::titleFont);
        titleText->setMinimumWidth(relaxationStyle::title::width);
        titleText->setMinimumHeightToFitText(relaxationStyle::title::maxLines);
        titleText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        titleText->setMargins(Margins(0, relaxationStyle::title::loopMarginTop, 0, 0));
        titleText->drawUnderline(false);

        loopedText = new TextFixedSize(mainVBox);
        loopedText->setFont(relaxationStyle::relStyle::loopedDescription::font);
        loopedText->setMaximumSize(relaxationStyle::relStyle::loopedDescription::maxSizeX,
                                   relaxationStyle::relStyle::loopedDescription::maxSizeY);
        loopedText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        loopedText->setMargins(Margins(0, relaxationStyle::relStyle::loopedDescription::marginTop, 0, 0));
        loopedText->setText(utils::translate("app_bell_relaxation_looped"));
        loopedText->drawUnderline(false);
        loopedText->setVisible(true);

        relaxationText = new TextFixedSize(mainVBox);
        relaxationText->setMaximumSize(relaxationStyle::relStyle::bottomDescription::maxSizeX,
                                       relaxationStyle::relStyle::bottomDescription::maxSizeY);
        relaxationText->setFont(relaxationStyle::relStyle::bottomDescription::font);
        relaxationText->setMargins(Margins(0, relaxationStyle::relStyle::bottomDescription::loopMarginTop, 0, 0));
        relaxationText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        relaxationText->setText(utils::translate("app_bellmain_relaxation"));
        relaxationText->drawUnderline(false);
        relaxationText->setVisible(true);
        relaxationText->activeItem = false;

        battery = new BellBattery(mainVBox, BatteryWidthMode::Fixed);
        battery->setMinimumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setMargins(Margins(0, relaxationStyle::relStyle::battery::loopMarginTop, 0, 0));
        battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        battery->setBatteryPercentMode(BatteryPercentMode::Show);
        battery->setEdges(RectangleEdge::None);
        battery->setVisible(false);

        updateBatteryStatus();
        mainVBox->resizeItems();
    }

    void RelaxationRunningLoopWindow::configureTimer()
    {
        auto timer = std::make_unique<app::ProgressTimer>(application,
                                                          *this,
                                                          relaxationLoopTimerName,
                                                          relaxationLoopTimerPeriod,
                                                          app::ProgressCountdownMode::Increasing);
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
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void RelaxationRunningLoopWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
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
        auto batteryStatus = presenter->getBatteryState();

        units::SOC soc = batteryStatus.level;
        auto state     = batteryStatus.state;

        if (soc < dischargingLevelShowTop) {
            battery->update(soc, presenter->isBatteryCharging(state));
            relaxationText->setVisible(false);
            battery->setVisible(true);
            battery->informContentChanged();
        }
        else {
            battery->setVisible(false);
            relaxationText->setVisible(true);
            relaxationText->informContentChanged();
        }

        return true;
    }

    void RelaxationRunningLoopWindow::handleError()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::UnsupportedMediaType);
        application->switchWindow(gui::window::name::audioErrorWindow, std::move(switchData));
    }

    void RelaxationRunningLoopWindow::handleDeletedFile()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::FileDeleted);
        application->switchWindow(gui::window::name::audioErrorWindow, std::move(switchData));
    }
} // namespace gui
