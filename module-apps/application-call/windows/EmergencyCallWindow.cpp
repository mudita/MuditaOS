// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAppStyle.hpp"
#include "EmergencyCallWindow.hpp"

#include <service-appmgr/Controller.hpp>

namespace gui
{

    EmergencyCallWindow::EmergencyCallWindow(app::ApplicationCommon *app, app::EnterNumberWindowInterface *interface)
        : NumberWindow(app, interface, app::window::name_emergencyCall)
    {
        buildInterface();
    }

    void EmergencyCallWindow::buildInterface()
    {
        using namespace callAppStyle;
        NumberWindow::buildInterface();

        navBar->setText(nav_bar::Side::Center, utils::translate("app_phonebook_ice_contacts_title"));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        numberDescriptionLabel->setText(utils::translate("app_call_emergency_text"));
    }

    status_bar::Configuration EmergencyCallWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        return appConfiguration;
    }

    bool EmergencyCallWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            // Call function
            if (inputEvent.is(KeyCode::KEY_LF)) {
                interface->handleEmergencyCallEvent(enteredNumber);
                return true;
            }
            else if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                auto data                        = std::make_unique<gui::SwitchData>();
                data->ignoreCurrentWindowOnStack = true;
                app::manager::Controller::sendAction(application,
                                                     app::manager::actions::ShowEmergencyContacts,
                                                     std::move(data),
                                                     app::manager::OnSwitchBehaviour::RunInBackground);
                return true;
            }
        }

        return NumberWindow::onInput(inputEvent);
    }

} /* namespace gui */
