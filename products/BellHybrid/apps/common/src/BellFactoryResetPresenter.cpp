// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellFactoryResetPresenter.hpp"

#include <service-appmgr/messages/OnboardingPowerDownRequest.hpp>
#include <SystemManager/SystemManagerCommon.hpp>

namespace gui
{
    BellFactoryResetPresenter::BellFactoryResetPresenter(app::ApplicationCommon *app) : application(app)
    {}

    void BellFactoryResetPresenter::powerOff([[maybe_unused]] sys::CloseReason reason)
    {
        sys::SystemManagerCommon::FactoryReset(application);
    }

    void BellFactoryResetPresenter::reboot()
    {
        sys::SystemManagerCommon::Reboot(application);
    }
} // namespace gui
