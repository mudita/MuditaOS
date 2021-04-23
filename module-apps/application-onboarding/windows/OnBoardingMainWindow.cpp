// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <InputEvent.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>

#include "module-apps/application-onboarding/data/OnBoardingSwitchData.hpp"
#include "OnBoardingMainWindow.hpp"

namespace app::onBoarding
{
    OnBoardingMainWindow::OnBoardingMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void OnBoardingMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::start));

        new gui::Image(this, 0, 0, 0, 0, "logo");
    }

    gui::top_bar::Configuration OnBoardingMainWindow::configureTopBar(gui::top_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::top_bar::Indicator::Time, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Battery, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::SimCard, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Signal, false);
        return appConfiguration;
    }

    bool OnBoardingMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            application->switchWindow(gui::window::name::onBoarding_languages,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<OnBoardingSwitchData>());

            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
