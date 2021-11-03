// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingWelcomeWindow.hpp"
#include "ApplicationBellOnBoarding.hpp"

#include <InputEvent.hpp>
#include <Icon.hpp>
#include <Application.hpp>

namespace gui
{
    OnBoardingWelcomeWindow::OnBoardingWelcomeWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer(app, name, displayDuration)
    {
        buildInterface();
    }

    void OnBoardingWelcomeWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void OnBoardingWelcomeWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);
        icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
    }

    void OnBoardingWelcomeWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            application->switchWindow(gui::window::name::onBoardingLanguageWindow);
            return true;
        };
    }

    bool OnBoardingWelcomeWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            application->switchWindow(gui::window::name::onBoardingLanguageWindow);
            return true;
        }
        return false;
    }

    void OnBoardingWelcomeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);

        icon->image->set("bell_big_logo", ImageTypeSpecifier::W_G);
        icon->text->setFont(style::window::font::verybiglight);
        icon->text->setText(utils::translate("app_bell_onboarding_welcome"));
    }
} // namespace gui
