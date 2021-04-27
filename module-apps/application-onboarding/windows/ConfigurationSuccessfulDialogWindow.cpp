// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ConfigurationSuccessfulDialogWindow.hpp"

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>
#include <module-apps/messages/DialogMetadataMessage.hpp>

namespace app::onBoarding
{
    ConfigurationSuccessfulDialogWindow::ConfigurationSuccessfulDialogWindow(app::Application *app)
        : gui::Dialog(app, gui::window::name::onBoarding_configuration_successful)
    {
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::start));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
    }

    bool ConfigurationSuccessfulDialogWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{utils::translate("app_onboarding_title_update_info"),
                                        "update_icon_W_G",
                                        utils::translate("app_onboarding_update_info"),
                                        "",
                                        [=]() -> bool { return true; }});
                application->switchWindow(
                    gui::window::name::onBoarding_update, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return true;
            }

            if (inputEvent.is(gui::KeyCode::KEY_RF)) {
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
