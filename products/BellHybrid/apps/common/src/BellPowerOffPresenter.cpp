// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellPowerOffPresenter.hpp"

#include <common/windows/BellWelcomeWindow.hpp>
#include <service-appmgr/messages/OnboardingPowerDownRequest.hpp>
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <system/Constants.hpp>

namespace gui
{
    BellPowerOffPresenter::BellPowerOffPresenter(app::ApplicationCommon *app) : application(app)
    {}

    void BellPowerOffPresenter::powerOff(sys::CloseReason reason)
    {
        switch (reason) {
        case sys::CloseReason::FactoryReset:
            sys::SystemManagerCommon::FactoryReset(application);
            break;
        case sys::CloseReason::OnboardingPowerDown: {
            auto msg = std::make_shared<app::OnboardingPowerDownRequest>();
            application->bus.sendUnicast(std::move(msg), service::name::system_manager);
        } break;
        default: {
            auto msg = std::make_shared<app::UserPowerDownRequest>();
            application->bus.sendUnicast(std::move(msg), service::name::system_manager);
        } break;
        }
    }
    void BellPowerOffPresenter::reboot()
    {
        sys::SystemManagerCommon::Reboot(application);
    }

} // namespace gui
