// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/ServiceDBDependencies.hpp>
#include <service-time/ServiceTimeName.hpp>
#include <string>
#include <vector>

namespace stm
{
    class ServiceTime;
}

namespace sys::dependencies
{
    template <>
    inline std::vector<std::string> getDependenciesFor<stm::ServiceTime>()
    {
        return sys::dependencies::getDependenciesTo<ServiceDB>();
    }
    template <>
    inline std::vector<std::string> getDependenciesTo<stm::ServiceTime>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<stm::ServiceTime>();
        dependencies.push_back(service::name::service_time);
        return dependencies;
    }
} // namespace sys::dependencies
