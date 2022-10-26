// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingShortcutsWindow.hpp"
#include "BellOnBoardingNames.hpp"

#include <Style.hpp>
#include <SideListView.hpp>
#include <gui/input/InputEvent.hpp>
#include <keymap/KeyMap.hpp>

namespace gui
{
    OnBoardingShortcutsWindow::OnBoardingShortcutsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::OnBoarding::OnBoardingShortcutsWindowContract::Presenter> &&presenter,
        const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void OnBoardingShortcutsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        auto layouts = presenter->getLayouts();
        spinner      = new WidgetSpinner(this, {layouts.begin(), layouts.end()}, Boundaries::Fixed);
        spinner->setSize(style::window_width, style::window_height);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        auto selectedLayout = presenter->getFirstLayout();
        spinner->setCurrentValue(selectedLayout);

        spinner->onValueChanged = [this](const auto &) {
            getApplication()->render(gui::RefreshModes::GUI_REFRESH_DEEP);
        };

        setFocusItem(spinner);
    }

    bool OnBoardingShortcutsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease()) {
            const auto key = mapKey(inputEvent.getKeyCode());
            if (key == KeyMap::LightPress) {
                auto currentLayout = spinner->getCurrentValue();
                if (presenter->isLastLayout(currentLayout)) {
                    getApplication()->switchWindow(window::name::onBoardingSettingsWindow);
                }
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
