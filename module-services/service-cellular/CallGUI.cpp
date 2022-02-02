// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallGUI.hpp"
#include "service-appmgr/data/CallActionsParams.hpp"
#include <service-appmgr/Controller.hpp>

CallGUI::CallGUI(sys::Service &s) : owner(s)
{}


void CallGUI::notifyRING()
{
        app::manager::Controller::sendAction(&owner,
                                             app::manager::actions::HandleIncomingCall,
                                             std::make_unique<app::manager::actions::CallParams>());
}

void CallGUI::notifyCLIP(const utils::PhoneNumber::View & number)
{
        app::manager::Controller::sendAction(&owner,
                                             app::manager::actions::HandleCallerId,
                                             std::make_unique<app::manager::actions::CallParams>(number));
}
