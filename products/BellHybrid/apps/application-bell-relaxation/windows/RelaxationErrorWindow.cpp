// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationErrorWindow.hpp"
#include <data/RelaxationSwitchData.hpp>
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>
#include <gui/widgets/Icon.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace gui
{
    RelaxationErrorWindow::RelaxationErrorWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::relaxation::RelaxationErrorContract::Presenter> &&presenter)
        : WindowWithTimer(app, gui::window::name::relaxationError, std::chrono::seconds{6}), presenter{
                                                                                                 std::move(presenter)}
    {
        buildInterface();
    }

    void RelaxationErrorWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void RelaxationErrorWindow::buildLayout()
    {
        statusBar->setVisible(false);

        auto icon = new Icon(this,
                             0,
                             0,
                             style::window_width,
                             style::window_height,
                             "big_information",
                             utils::translate("app_bell_relaxation_error_message"),
                             ImageTypeSpecifier::W_G);
        icon->image->setMargins({0, gui::relaxationStyle::error::imageMarginTop, 0, 0});
        icon->text->setFont(style::window::font::verybiglight);
        const auto textPadding = icon->text->getPadding();
        icon->text->setPadding(
            {textPadding.left, gui::relaxationStyle::error::textPaddingTop, textPadding.right, textPadding.bottom});
        icon->text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));

        icon->resizeItems();
    }

    void RelaxationErrorWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            application->switchWindow(gui::name::window::main_window);
            return true;
        };
    }

    bool RelaxationErrorWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        return true;
    }
} // namespace gui
