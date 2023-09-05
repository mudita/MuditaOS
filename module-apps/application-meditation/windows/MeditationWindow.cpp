// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"
#include "IntervalBox.hpp"
#include "MeditationTimerData.hpp"
#include "MeditationWindow.hpp"
#include "Names.hpp"
#include "Style.hpp"
#include "TimerProperty.hpp"

#include <gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>

#include <cassert>

namespace gui
{
    MeditationWindow::MeditationWindow(app::ApplicationCommon *app, const MeditationParams &params)
        : AppWindow{app, name::window::main_window}, meditationParams(params)
    {
        MeditationWindow::buildInterface();
    }

    void MeditationWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void MeditationWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_meditation_title_main"));

        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::options));
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::start));

        timeSetter = new TimerProperty(this,
                                       style::meditation::timer::X,
                                       style::meditation::timer::Y,
                                       2 * style::meditation::timer::Radius,
                                       2 * style::meditation::timer::Radius);
        if (!timeSetter->setTime(meditationParams.meditationDuration.initValue)) {
            LOG_ERROR("Incorrect meditation duration value! The default value is set.");
        }
        timeSetter->setOnChangeCallback(meditationParams.meditationDuration.onChangeCallback);
        timeSetter->setEdges(RectangleEdge::None);
        setFocusItem(timeSetter);

        intervalBox = new IntervalBox(this,
                                      style::meditation::intervalBox::X,
                                      style::meditation::intervalBox::Y,
                                      style::meditation::intervalBox::Width,
                                      style::text_spinner_label::h,
                                      timeSetter);
        if (!intervalBox->setIntervalValue(meditationParams.intervalChime.initValue)) {
            LOG_ERROR("Incorrect chime interval value! The default value is set.");
        }
        intervalBox->setOnChangeCallback(meditationParams.intervalChime.onChangeCallback);
        intervalBox->setEdges(RectangleEdge::None);

        intervalBox->setNavigationItem(NavigationDirection::UP, timeSetter);
        intervalBox->setNavigationItem(NavigationDirection::DOWN, timeSetter);
        timeSetter->setNavigationItem(NavigationDirection::DOWN, intervalBox);
        timeSetter->setNavigationItem(NavigationDirection::UP, intervalBox);
    }

    void MeditationWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    auto MeditationWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(KeyCode::KEY_LF) && navBar->isActive(nav_bar::Side::Left)) {
                application->switchWindow(app::window::name::meditation_options);
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER) && navBar->isActive(nav_bar::Side::Center)) {
                auto app = dynamic_cast<app::ApplicationMeditation *>(application);
                assert(app);

                auto timerSwitchData = std::make_unique<MeditationTimerData>(app->state->getPreparationTime(),
                                                                             timeSetter->getTime(),
                                                                             intervalBox->getIntervalValue(),
                                                                             app->state->isCounterVisible());
                application->switchWindow(app::window::name::meditation_timer, std::move(timerSwitchData));
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void MeditationWindow::invalidate() noexcept
    {
        timeSetter  = nullptr;
        intervalBox = nullptr;
    }
} // namespace gui
