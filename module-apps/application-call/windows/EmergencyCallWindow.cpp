// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "../data/CallSwitchData.hpp"
#include "../ApplicationCall.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>

#include <i18n/i18n.hpp>
#include "EmergencyCallWindow.hpp"

#include <cassert>

namespace gui
{

    EmergencyCallWindow::EmergencyCallWindow(app::Application *app, app::EnterNumberWindowInterface *interface)
        : EnterNumberWindow(app, interface, app::window::name_emergencyCall)
    {
        assert(app != nullptr);
        assert(interface != nullptr);
        numberLabel->setText(utils::localize.get("app_call_emergency"));
    }

    void EmergencyCallWindow::rebuild()
    {}
    void EmergencyCallWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }
    void EmergencyCallWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    bool EmergencyCallWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = EnterNumberWindow::onInput(inputEvent);

        //	if( number.empty() ) {
        //		numberLabel->setText( utils::localize.get("app_call_emergency") );
        //	}

        return ret;
    }
    bool EmergencyCallWindow::handleSwitchData(SwitchData *data)
    {
        bool ret = EnterNumberWindow::handleSwitchData(data);

        return ret;
    }

} /* namespace gui */
