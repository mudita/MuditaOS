// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <InputEvent.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <module-apps/windows/DialogMetadata.hpp>
#include <module-apps/messages/DialogMetadataMessage.hpp>
#include "module-apps/application-onboarding/data/OnBoardingSwitchData.hpp"
#include "module-apps/application-onboarding/ApplicationOnBoarding.hpp"

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
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(::style::strings::common::skip));

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
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{utils::localize.get("app_onboarding_title_configuration"),
                                        "success_icon_W_G",
                                        utils::localize.get("app_onboarding_configuration_successful"),
                                        "",
                                        [=]() -> bool { return true; }});
                application->switchWindow(gui::window::name::onBoarding_configuration_successful,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::move(metaData));
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF)) {

                auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                    utils::localize.get("app_onboarding_title"),
                    "info_icon_W_G",
                    utils::localize.get("app_onboarding_skip_confirm"),
                    "",
                    [=]() -> bool {
                        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                            gui::DialogMetadata{utils::localize.get("app_onboarding_title_configuration"),
                                                "info_icon_W_G",
                                                utils::localize.get("app_onboarding_no_configuration"),
                                                "",
                                                [=]() -> bool { return true; }});

                        application->switchWindow(gui::window::name::onBoarding_no_configuration,
                                                  gui::ShowMode::GUI_SHOW_INIT,
                                                  std::move(metaData));
                        return true;
                    }});

                application->switchWindow(
                    gui::window::name::onBoarding_skip, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            }
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace app::onBoarding
