// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <InputEvent.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>

#include "module-apps/application-onboarding/data/OnBoardingSwitchData.hpp"
#include "OnBoardingMainWindow.hpp"

namespace gui
{
    OnBoardingMainWindow::OnBoardingMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void OnBoardingMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(::style::strings::common::start));

        new Image(this, 0, 0, 0, 0, "logo");
    }

    top_bar::Configuration OnBoardingMainWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.set(top_bar::Indicator::Time, false);
        appConfiguration.set(top_bar::Indicator::Battery, false);
        appConfiguration.set(top_bar::Indicator::SimCard, false);
        appConfiguration.set(top_bar::Indicator::Signal, false);
        return appConfiguration;
    }

    bool OnBoardingMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_ENTER)) {
            application->switchWindow(gui::window::name::onBoarding_languages,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<gui::OnBoardingSwitchData>());

            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
