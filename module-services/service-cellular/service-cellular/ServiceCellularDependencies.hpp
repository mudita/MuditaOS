// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-antenna/ServiceAntennaDependencies.hpp>
#include <service-cellular/ServiceCellularName.hpp>
#include <string>
#include <vector>

class ServiceCellular;

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceCellular>()
    {
        return sys::dependencies::getDependenciesTo<ServiceAntenna>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceCellular>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<ServiceCellular>();
        dependencies.push_back(service::name::cellular);
        return dependencies;
    }
} // namespace sys::dependencies
