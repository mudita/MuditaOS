// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewFeaturesWindow.hpp"

#include <SideListView.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>
#include <common/windows/BellFinishedWindow.hpp>

namespace
{
    constexpr auto endWindowTimeout = std::chrono::seconds{5};
}

namespace app::whatsnew
{
    using namespace gui;

    WhatsNewFeaturesWindow::WhatsNewFeaturesWindow(app::ApplicationCommon *app,
                                                   std::unique_ptr<WhatsNewFeaturesContract::Presenter> &&presenter,
                                                   const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    auto WhatsNewFeaturesWindow::buildInterface() -> void
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        layouts = presenter->getLayouts();

        itemSpinner = new WidgetSpinner(this, {layouts.begin(), layouts.end()}, Boundaries::Fixed);
        itemSpinner->setSize(style::window_width, style::window_height);
        itemSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        itemSpinner->setFocusEdges(RectangleEdge::None);
        itemSpinner->setCurrentValue(presenter->getFirstLayout());

        itemSpinner->onValueChanged = [this]([[maybe_unused]] const auto &value) {
            getApplication()->render(gui::RefreshModes::GUI_REFRESH_DEEP);
        };

        setFocusItem(itemSpinner);
    }

    auto WhatsNewFeaturesWindow::onInput(const gui::InputEvent &inputEvent) -> bool
    {
        /* Prevent leaving by long-pressing back */
        if (inputEvent.isLongRelease(gui::KeyCode::KEY_RF)) {
            return true;
        }
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) ||
            (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER) && isLastLayout())) {
            presenter->setCurrentOsVersion();
            switchToEndWindow();
            return true;
        }
        if (itemSpinner->onInput(inputEvent)) {
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    auto WhatsNewFeaturesWindow::isLastLayout() -> bool
    {
        return itemSpinner->getCurrentValue() == layouts.back();
    }

    auto WhatsNewFeaturesWindow::switchToEndWindow() -> void
    {
        using ExitBehaviour = gui::BellFinishedWindowData::ExitBehaviour;
        application->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("big_namaste_W_G",
                                                         "",
                                                         utils::translate("app_bell_whatsnew_end_screen_text"),
                                                         ExitBehaviour::ReturnToHomescreen,
                                                         endWindowTimeout));
    }
} // namespace app::whatsnew
