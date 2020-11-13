// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-services/service-appmgr/service-appmgr/Actions.hpp>
#include <module-services/service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/data/AppDesktopActionParams.hpp>

#include <module-services/service-cellular/service-cellular/CellularMessage.hpp>

namespace gui
{
    class LockTester
    {
      public:
        static std::shared_ptr<CellularSimGetPinRequest> getRequestPinAction(unsigned int attempts)
        {
            return std::make_shared<CellularSimGetPinRequest>(Store::GSM::SIM::SIM1, attempts, "DUPA");
        }
        static std::shared_ptr<CellularSimGetPukRequest> getRequestPukAction(unsigned int attempts)
        {
            return std::make_shared<CellularSimGetPukRequest>(Store::GSM::SIM::SIM1, attempts, "DUPA");
        }
        static std::shared_ptr<CellularSimChangePinRequest> getSimChangePinAction(unsigned int attempts)
        {
            return std::make_shared<CellularSimChangePinRequest>(Store::GSM::SIM::SIM1, attempts, "DUPA");
        }
        static std::shared_ptr<CellularSimBlockedMessage> getSimBlockedAction()
        {
            return std::make_shared<CellularSimBlockedMessage>(Store::GSM::SIM::SIM1);
        }
        static std::shared_ptr<CellularSimUnlockedMessage> getSimUnlockedAction()
        {
            return std::make_shared<CellularSimUnlockedMessage>(Store::GSM::SIM::SIM1);
        }
        static std::shared_ptr<CellularUnhandledCMEMessage> getUnhandledCMEAction(unsigned int attempts)
        {
            return std::make_shared<CellularUnhandledCMEMessage>(Store::GSM::SIM::SIM1, attempts);
        }
    };
} // namespace gui
