// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EmergencyCallWindow.hpp"

#include "application-call/data/CallAppStyle.hpp"

#include <service-appmgr/Controller.hpp>

namespace gui
{

    EmergencyCallWindow::EmergencyCallWindow(app::Application *app, app::EnterNumberWindowInterface *interface)
        : NumberWindow(app, interface, app::window::name_emergencyCall)
    {
        buildInterface();
    }

    void EmergencyCallWindow::buildInterface()
    {
        using namespace callAppStyle;
        NumberWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_ice_contacts_title"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        numberDescriptionLabel->setText(utils::localize.get("app_call_emergency_text"));
    }

    bool EmergencyCallWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            auto data                        = std::make_unique<gui::SwitchData>();
            data->ignoreCurrentWindowOnStack = true;
            app::manager::Controller::sendAction(
                application, app::manager::actions::ShowEmergencyContacts, std::move(data));
            return true;
        }

        return NumberWindow::onInput(inputEvent);
    }

} /* namespace gui */
