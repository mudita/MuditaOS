// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/ShortcutsWindow.hpp"

#include <Style.hpp>
#include <SideListView.hpp>
#include <gui/input/InputEvent.hpp>

namespace gui
{
    ShortcutsWindow::ShortcutsWindow(app::ApplicationCommon *app,
                                     std::unique_ptr<ShortcutsWindowContract::Presenter> &&presenter,
                                     const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void ShortcutsWindow::buildInterface()
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

    bool ShortcutsWindow::isOneOfTwoLastShortcuts() const
    {
        auto currentLayout = spinner->getCurrentValue();
        return presenter->isOneOfTwoLastLayouts(currentLayout);
    }

    bool ShortcutsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }

        if (presenter->onInput(inputEvent, spinner->getCurrentValue())) {
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
