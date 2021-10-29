// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellPowerOffPresenter.hpp"

#include <common/windows/BellWelcomeWindow.hpp>
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <system/Constants.hpp>

namespace gui
{
    BellPowerOffPresenter::BellPowerOffPresenter(app::ApplicationCommon *app) : application(app)
    {}

    void BellPowerOffPresenter::powerOff()
    {
        auto msg = std::make_shared<app::UserPowerDownRequest>();
        application->bus.sendUnicast(std::move(msg), service::name::system_manager);
    }

    void BellPowerOffPresenter::factoryReset()
    {
        sys::SystemManagerCommon::FactoryReset(application);
    }

    void BellPowerOffPresenter::reboot()
    {
        sys::SystemManagerCommon::Reboot(application);
    }

} // namespace gui
