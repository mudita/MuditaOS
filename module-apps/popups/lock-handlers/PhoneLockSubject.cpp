//// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
//// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/service-appmgr/model/ApplicationManager.hpp"
#include "popups/data/LockData.hpp"
#include "popups/data/LockMessages.hpp"
#include "PhoneLockSubject.hpp"

namespace lock
{

    PhoneLockSubject::PhoneLockSubject(sys::Service *owner) : owner{owner}
    {
        if (owner == nullptr) {
            throw std::invalid_argument{"Subject's owner is invalid"};
        }
    }

    void PhoneLockSubject::unlock()
    {
        LOG_ERROR("Subject chce odblokowac!");

        owner->bus.sendUnicast(std::make_shared<UnlockPhone>(), app::manager::ApplicationManager::ServiceName);
    }

    void PhoneLockSubject::lock()
    {
        LOG_ERROR("Subject chce zablokowac!");

        owner->bus.sendUnicast(std::make_shared<LockPhone>(), app::manager::ApplicationManager::ServiceName);
    }

} // namespace lock
