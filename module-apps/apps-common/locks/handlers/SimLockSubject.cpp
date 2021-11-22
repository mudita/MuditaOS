// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockSubject.hpp"

#include <apps-common/locks/data/PhoneLockMessages.hpp>
#include <apps-common/locks/data/SimLockMessages.hpp>
#include <service-appmgr/Constants.hpp>

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
        owner->bus.sendUnicast(std::make_shared<locks::SetSim>(simSlot), service::name::appmgr);
    }

    void SimLockSubject::resetSimLockState()
    {
        owner->bus.sendUnicast(std::make_shared<locks::ResetSimLockState>(), service::name::appmgr);
    }

    void SimLockSubject::changeSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::ChangeSimPin>(), service::name::appmgr);
    }

    void SimLockSubject::enableSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::EnableSimPin>(), service::name::appmgr);
    }

    void SimLockSubject::disableSimPin()
    {
        owner->bus.sendUnicast(std::make_shared<locks::DisableSimPin>(), service::name::appmgr);
    }

    void SimLockSubject::verifyInput(LockInput inputData)
    {
        owner->bus.sendUnicast(std::make_shared<UnLockSimInput>(inputData), service::name::appmgr);
    }
} // namespace locks
