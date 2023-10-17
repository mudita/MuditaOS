// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DBServiceName.hpp>
#include <Service/ServiceDependencies.hpp>

#include <string>
#include <vector>

class ServiceDB;

namespace sys::dependencies
{
    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceDB>()
    {
        return {};
    }
    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceDB>()
    {
        return {service::name::db};
    }
} // namespace sys::dependencies
