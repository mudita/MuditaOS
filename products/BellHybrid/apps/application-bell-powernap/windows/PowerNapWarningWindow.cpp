// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerNapWarningWindow.hpp"
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>

#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <common/data/StyleCommon.hpp>

namespace
{
    constexpr std::chrono::seconds screenTimeout{5};
} // namespace

namespace gui
{
    PowerNapWarningWindow::PowerNapWarningWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::powernap::PowerNapWarningContract::Presenter> presenter)
        : WindowWithTimer(app, window::name::powerNapWarning, screenTimeout), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void PowerNapWarningWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
        resetTimer(screenTimeout);
    }

    void PowerNapWarningWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "bell_status_battery_lvl0",
                        {},
                        gui::ImageTypeSpecifier::W_G);
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins({0, gui::bell_style::warning_icon_top_margin, 0, 0});
        icon->text->setMaximumWidth(gui::bell_style::warning_text_width);
        icon->text->setFont(style::window::font::verybiglight);
        icon->text->setRichText(utils::translate("app_bell_alarm_low_battery_info"));
    }

    bool PowerNapWarningWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            presenter->activate();
            return true;
        }
        return false;
    }

    void PowerNapWarningWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }
} // namespace gui
