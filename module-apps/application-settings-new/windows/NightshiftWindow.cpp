// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NightshiftWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <module-apps/application-settings-new/widgets/SettingsStyle.hpp>

namespace gui
{
    NightshiftWindow::NightshiftWindow(app::Application *app) : AppWindow(app, gui::window::name::nightshift)
    {
        buildInterface();
    }

    void NightshiftWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_settings_title_nightshift"));

        body = new gui::VBox(this,
                             style::window::default_left_margin,
                             style::settings::window::nightshift::body_offset,
                             style::window::default_body_width,
                             style::window::default_body_height);
        body->setEdges(gui::RectangleEdge::None);

        fromTimeWidget = new TimeWidget(
            body,
            utils::localize.get("app_settings_nightshift_from"),
            [this](const UTF8 &text) { bottomBarTemporaryMode(text, false); },
            [this]() { bottomBarRestoreFromTemporaryMode(); });
        fromTimeWidget->activeItem = true;

        separatorLabel = new gui::Label(
            body, 0, 0, style::window::default_body_width, style::settings::window::nightshift::separator_h);
        separatorLabel->setEdges(gui::RectangleEdge::None);
        separatorLabel->activeItem = false;

        toTimeWidget = new TimeWidget(
            body,
            utils::localize.get("app_settings_nightshift_to"),
            [this](const UTF8 &text) { bottomBarTemporaryMode(text, false); },
            [this]() { bottomBarRestoreFromTemporaryMode(); });
        toTimeWidget->activeItem = true;

        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setFocusItem(fromTimeWidget);
    }

    auto NightshiftWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            auto timeFrom = fromTimeWidget->getTime();
            LOG_DEBUG("timeFrom in minutes: %d", static_cast<int>(timeFrom.count()));
            auto timeTo = toTimeWidget->getTime();
            LOG_DEBUG("timeTo in minutes: %d", static_cast<int>(timeTo.count()));
            application->switchWindow(gui::window::name::torch);
            return true;
        }

        return false;
    }
} // namespace gui
