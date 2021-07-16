// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockSubject.hpp"

#include <service-appmgr/model/ApplicationManager.hpp>
#include <locks/data/PhoneLockMessages.hpp>

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

    void PhoneLockSubject::enablePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<EnablePhoneLock>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::disablePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<DisablePhoneLock>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::changePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<ChangePhoneLock>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::setPhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<SetPhoneLock>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::skipSetPhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<SkipSetPhoneLock>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::verifyInput(LockInput inputData)
    {
        owner->bus.sendUnicast(std::make_shared<UnLockPhoneInput>(inputData),
                               app::manager::ApplicationManager::ServiceName);
    }
} // namespace locks
