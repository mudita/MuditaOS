// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ConfigurationSuccessfulDialogWindow.hpp"

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <header/IceAction.hpp>
#include <service-appmgr/Controller.hpp>

#include <i18n/i18n.hpp>
#include <Style.hpp>

namespace app::onBoarding
{
    ConfigurationSuccessfulDialogWindow::ConfigurationSuccessfulDialogWindow(app::ApplicationCommon *app)
        : gui::Dialog(app, gui::window::name::onBoarding_configuration_successful)
    {
        header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::start));
        navBar->setActive(gui::nav_bar::Side::Right, false);
    }

    bool ConfigurationSuccessfulDialogWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->switchWindow(gui::window::name::onBoarding_update, gui::ShowMode::GUI_SHOW_INIT);
                return true;
            }

            if (inputEvent.is(gui::KeyCode::KEY_RF)) {
                return true;
            }

            if (inputEvent.is(gui::KeyCode::KEY_LEFT)) {
                app::manager::Controller::sendAction(application,
                                                     app::manager::actions::EmergencyDial,
                                                     std::make_unique<gui::SwitchData>(),
                                                     app::manager::OnSwitchBehaviour::RunInBackground);
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
