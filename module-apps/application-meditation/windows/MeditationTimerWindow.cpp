// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationTimerData.hpp"
#include "MeditationTimerWindow.hpp"
#include "Names.hpp"
#include "Style.hpp"

#include <gui/core/FontManager.hpp>
#include <i18n/i18n.hpp>
#include <Timers/TimerFactory.hpp>

#include <cassert>

using namespace gui;

namespace
{
    constexpr std::chrono::seconds endScreenTimeoutTime{5};
} // namespace

MeditationTimerWindow::MeditationTimerWindow(app::ApplicationCommon *app)
    : AppWindow{app, name::window::main_window},
      endScreenTimeout{sys::TimerFactory::createSingleShotTimer(
          app, "MeditationEndScreenTimeout", endScreenTimeoutTime, [this](sys::Timer &) {
              application->switchWindow(app::window::name::meditation_main_window);
          })}
{
    MeditationTimerWindow::buildInterface();
}

void MeditationTimerWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void MeditationTimerWindow::buildInterface()
{
    auto app = dynamic_cast<app::ApplicationMeditation *>(application);
    assert(app != nullptr); // Pre-condition check.

    AppWindow::buildInterface();
    setTitle(utils::translate("app_meditation_title_main"));

    timer = new MeditationTimer(style::meditation::timer::X,
                                style::meditation::timer::Y,
                                2 * style::meditation::timer::Radius,
                                2 * style::meditation::timer::Radius,
                                app,
                                this);

    meditationInfo = new Text(this,
                              style::meditation::timer::infoText::X,
                              style::meditation::timer::infoText::Y,
                              style::meditation::timer::infoText::Width,
                              style::meditation::timer::infoText::Height);
    meditationInfo->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
}

void MeditationTimerWindow::destroyInterface()
{
    erase();
    invalidate();
}

void MeditationTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto timerData = dynamic_cast<MeditationTimerData *>(data);
    if (timerData != nullptr) {
        setVisiblePreparation();
        meditationTime           = timerData->getMeditationTime();
        meditationIntervalPeriod = timerData->getMeditationIntervals();

        auto onPreparation = [&]() -> void {
            setVisibleRunning();
            auto onMeditationEnd = [&]() -> void {
                setVisibleMeditationEnd();
                application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
                timer->playSound();
            };
            timer->getTimer().registerOnFinishedCallback(onMeditationEnd);
            timer->getTimer().reset(meditationTime, meditationIntervalPeriod);
            timer->getTimer().start();
            timer->getProgress().setMaximum(meditationTime.count());
            timer->playSound();
            application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
        };
        timer->getTimer().registerOnFinishedCallback(onPreparation);
        timer->getTimer().reset(timerData->getPreparationTime());
        timer->getTimer().start();
        timer->getProgress().setMaximum(timerData->getPreparationTime().count());
        timer->setCounterVisible(timerData->isCounterEnabled());
    }
}

auto MeditationTimerWindow::onInput(const InputEvent &inputEvent) -> bool
{
    if (inputEvent.isShortRelease()) {
        if (finished) {
            endScreenTimeout.stop();
            application->switchWindow(app::window::name::meditation_main_window);
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_RF) && navBar->isActive(nav_bar::Side::Right)) {
            timer->getTimer().stop();
            setVisibleMeditationEnd();
            return true;
        }
        if (inputEvent.is(KeyCode::KEY_ENTER) && navBar->isActive(nav_bar::Side::Center)) {
            if (timer->getTimer().isStopped()) {
                timer->getTimer().start();
                setVisibleRunning();
            }
            else {
                timer->getTimer().stop();
                setVisiblePaused();
            }
            return true;
        }
    }

    return AppWindow::onInput(inputEvent);
}

void MeditationTimerWindow::setWidgetVisible(bool sBar, bool bBar, bool counter)
{
    applyToStatusBar([sBar](status_bar::Configuration configuration) {
        configuration.setIndicator(status_bar::Indicator::Time, sBar);
        return configuration;
    });

    header->setTitleVisibility(sBar);
    navBar->setActive(nav_bar::Side::Center, bBar);
    navBar->setActive(nav_bar::Side::Left, bBar);
    timer->setVisible(counter);
}
void MeditationTimerWindow::setVisibleRunning()
{
    setWidgetVisible(false, true, true);
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::pause));
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::stop));
    meditationInfo->setVisible(false);
}
void MeditationTimerWindow::setVisiblePaused()
{
    setWidgetVisible(true, true, true);
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::resume));
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::stop));
}
void MeditationTimerWindow::setVisiblePreparation()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::translate("app_meditation_put_down_phone_and_wait"), &format);
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);

    finished = false;
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
}

void MeditationTimerWindow::setVisibleMeditationEnd()
{
    setWidgetVisible(false, false, false);
    TextFormat format(FontManager::getInstance().getFont(style::window::font::biglight));
    gui::text::RichTextParser parser;
    auto textParsed = parser.parse(utils::translate("app_meditation_thank_you_for_session"), &format);
    finished        = true;
    meditationInfo->setText(std::move(textParsed));
    meditationInfo->setVisible(true);
    navBar->setVisible(false);
    endScreenTimeout.start();
}

void MeditationTimerWindow::invalidate() noexcept
{
    timer          = nullptr;
    meditationInfo = nullptr;
}
