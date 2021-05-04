//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockSubject.hpp"

#include <service-appmgr/service-appmgr/model/ApplicationManager.hpp>
#include <locks/data/LockMessages.hpp>

namespace locks
{
    PhoneLockSubject::PhoneLockSubject(sys::Service *owner) : owner{owner}
    {
        if (owner == nullptr) {
            throw std::invalid_argument{"Subject's owner is invalid"};
        }
    }

    void PhoneLockSubject::unlock()
    {
        owner->bus.sendUnicast(std::make_shared<UnlockPhone>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::lock()
    {
        owner->bus.sendUnicast(std::make_shared<LockPhone>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::verifyInput(const std::vector<unsigned int> &inputData)
    {
        owner->bus.sendUnicast(std::make_shared<UnLockPhoneInput>(inputData),
                               app::manager::ApplicationManager::ServiceName);
    }
} // namespace lock
