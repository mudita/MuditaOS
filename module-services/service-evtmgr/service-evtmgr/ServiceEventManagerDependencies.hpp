// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceEventManagerName.hpp"
#include <service-db/ServiceDBDependencies.hpp>

#include <string>
#include <vector>

class EventManager;

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<EventManager>()
    {
        return sys::dependencies::getDependenciesTo<ServiceDB>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<EventManager>()
    {
        auto dependencies = getDependenciesFor<EventManager>();
        dependencies.push_back(service::name::evt_manager);
        return dependencies;
    }
} // namespace sys::dependencies
