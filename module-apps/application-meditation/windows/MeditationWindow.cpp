// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
    MeditationWindow::MeditationWindow(app::Application *app) : AppWindow{app, name::window::main_window}
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

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::options));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::start));

        timeSetter = new TimerProperty(this,
                                       style::meditation::timer::X,
                                       style::meditation::timer::Y,
                                       style::meditation::timer::Width,
                                       style::meditation::timer::Height);
        timeSetter->setEdges(RectangleEdge::None);
        setFocusItem(timeSetter);

        intervalBox = new IntervalBox(this,
                                      style::meditation::intervalBox::X,
                                      style::meditation::intervalBox::Y,
                                      style::meditation::intervalBox::Width,
                                      style::meditation::intervalBox::Height,
                                      timeSetter);
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
            if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
                application->switchWindow(app::window::name::meditation_options);
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
                auto app = dynamic_cast<app::ApplicationMeditation *>(application);
                assert(app);

                auto timerSwitchData = std::make_unique<MeditationTimerData>(app->state->preparationTime,
                                                                             timeSetter->getTime(),
                                                                             intervalBox->getIntervalValue(),
                                                                             app->state->showCounter);
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
