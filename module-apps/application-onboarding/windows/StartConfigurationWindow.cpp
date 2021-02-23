// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <InputEvent.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include "module-apps/application-onboarding/data/OnBoardingSwitchData.hpp"

#include "StartConfigurationWindow.hpp"

namespace app::onBoarding
{
    StartConfigurationWindow::StartConfigurationWindow(app::Application *app)
        : gui::AppWindow(app, gui::window::name::onBoarding_start_configuration)
    {
        buildInterface();
    }

    void StartConfigurationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(::style::strings::common::start));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(::style::strings::common::back));

        new gui::Icon(this,
                      0,
                      0,
                      style::window_width,
                      style::window::default_body_height,
                      "logo_no_text",
                      utils::localize.get("app_onboarding_start_configuration"));
    }

    bool StartConfigurationWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(gui::KeyCode::KEY_RF)) {
                application->switchWindow(gui::window::name::onBoarding_eula,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::make_unique<OnBoardingSwitchData>());
            }
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
