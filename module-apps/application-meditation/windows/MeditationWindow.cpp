#include "MeditationWindow.hpp"

#include "application-meditation/windows/Names.hpp"
#include "application-meditation/widgets/IntervalBox.hpp"
#include "application-meditation/widgets/CircularTimeSelector.hpp"
#include "application-meditation/ApplicationMeditation.hpp"
#include "application-meditation/data/Style.hpp"
#include "application-meditation/data/MeditationTimerData.hpp"
#include "application-meditation/ApplicationMeditation.hpp"

#include "InputEvent.hpp"

#include <cassert>

#include <i18/i18.hpp>

#include <iostream>
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
        auto app = dynamic_cast<app::ApplicationMeditation *>(application);
        assert(app != nullptr); // Pre-condition check.

        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_meditation_title_main"));

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setText(BottomBar::Side::CENTER, "start");

        timeSelector = new CircularTimeSelector(this,
                                                style::meditation::timer::X,
                                                style::meditation::timer::Y,
                                                style::meditation::timer::Width,
                                                style::meditation::timer::Height);

        intervalBox = new IntervalBox(this,
                                      style::window::default_left_margin,
                                      style::meditation::timer::Y + style::meditation::timer::Height,
                                      style::window::default_body_width,
                                      80);

        setFocusItem(timeSelector);

        intervalBox->setNavigationItem(NavigationDirection::UP, timeSelector);
        intervalBox->setNavigationItem(NavigationDirection::DOWN, timeSelector);
        timeSelector->setNavigationItem(NavigationDirection::DOWN, intervalBox);
        timeSelector->setNavigationItem(NavigationDirection::UP, intervalBox);
    }

    void MeditationWindow::destroyInterface()
    {
        erase();
    }
    auto MeditationWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_LF) && bottomBar->isActive(BottomBar::Side::LEFT)) {
                application->switchWindow(app::window::name::meditation_options);
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER) && bottomBar->isActive(BottomBar::Side::CENTER)) {
                auto app = reinterpret_cast<app::ApplicationMeditation *>(application);
                assert(app);
                std::cout << "HERE I WORK onInput timeSelector->getTimeInSeconds() = "
                          << timeSelector->getTimeInSeconds() << std::endl;
                auto waitInfoData = std::make_unique<MeditationTimerData>(
                    app->state->preparationTimeInSeconds, timeSelector->getTimeInSeconds(), 0, app->state->showCounter);
                application->switchWindow(app::window::name::meditation_timer, std::move(waitInfoData));
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
        ;
    }
} // namespace gui
