// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockSubject.hpp"

#include <locks/data/PhoneLockMessages.hpp>
#include <service-appmgr/service-appmgr/model/ApplicationManager.hpp>

namespace locks
{
    SimLockSubject::SimLockSubject(sys::Service *owner) : owner{owner}
    {
        if (owner == nullptr) {
            throw std::invalid_argument{"Subject's owner is invalid"};
        }
    }

    void SimLockSubject::setSim(cellular::api::SimSlot simSlot)
    {
        owner->bus.sendUnicast(std::make_shared<locks::SetSim>(simSlot), app::manager::ApplicationManager::ServiceName);
    }

    void SimLockSubject::changeSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::ChangeSimPin>(), app::manager::ApplicationManager::ServiceName);
    }

    void SimLockSubject::enableSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::EnableSimPin>(), app::manager::ApplicationManager::ServiceName);
    }

    void SimLockSubject::disableSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::DisableSimPin>(), app::manager::ApplicationManager::ServiceName);
    }

    void SimLockSubject::verifyInput(LockInput inputData)
    {
        owner->bus.sendUnicast(std::make_shared<UnLockSimInput>(inputData),
                               app::manager::ApplicationManager::ServiceName);
    }
} // namespace locks
