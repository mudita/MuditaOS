// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/popups/lock-widgets/Lock.hpp"

#include <module-services/service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/Actions.hpp>

namespace lock
{
    class LockHandler
    {
      protected:
        virtual ~LockHandler() = default;

        gui::Lock *lock = nullptr;
    };
} // namespace lock
