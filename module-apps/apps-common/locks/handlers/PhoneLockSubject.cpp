// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockSubject.hpp"

#include <service-appmgr/Constants.hpp>
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
        owner->bus.sendUnicast(std::make_shared<UnlockPhone>(), service::name::appmgr);
    }

    void PhoneLockSubject::lock()
    {
        owner->bus.sendUnicast(std::make_shared<LockPhone>(), service::name::appmgr);
    }

    void PhoneLockSubject::enablePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<EnablePhoneLock>(), service::name::appmgr);
    }

    void PhoneLockSubject::disablePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<DisablePhoneLock>(), service::name::appmgr);
    }

    void PhoneLockSubject::changePhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<ChangePhoneLock>(), service::name::appmgr);
    }

    void PhoneLockSubject::setPhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<SetPhoneLock>(), service::name::appmgr);
    }

    void PhoneLockSubject::skipSetPhoneLock()
    {
        owner->bus.sendUnicast(std::make_shared<SkipSetPhoneLock>(), service::name::appmgr);
    }

    void PhoneLockSubject::verifyInput(LockInput inputData)
    {
        owner->bus.sendUnicast(std::make_shared<UnLockPhoneInput>(inputData), service::name::appmgr);
    }
} // namespace locks
