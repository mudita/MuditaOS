#include "MeditationWindow.hpp"

#include <cassert>

#include <i18/i18.hpp>

#include <application-meditation/data/Style.hpp>

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

        timer = new MeditationTimer(style::meditation::timer::X,
                                    style::meditation::timer::Y,
                                    style::meditation::timer::Width,
                                    style::meditation::timer::Height,
                                    app,
                                    this);

        setFocusItem(timer);
    }

    void MeditationWindow::destroyInterface()
    {
        erase();
        timer = nullptr;
    }

    auto MeditationWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
