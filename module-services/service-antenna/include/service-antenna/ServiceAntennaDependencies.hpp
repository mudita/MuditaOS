// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/ServiceEventManagerDependencies.hpp>
#include <service-antenna/ServiceAntennaName.hpp>
#include <string>
#include <vector>

class ServiceAntenna;

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceAntenna>()
    {
        return sys::dependencies::getDependenciesTo<EventManager>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceAntenna>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<ServiceAntenna>();
        dependencies.push_back(service::name::antenna);
        return dependencies;
    }
} // namespace sys::dependencies
