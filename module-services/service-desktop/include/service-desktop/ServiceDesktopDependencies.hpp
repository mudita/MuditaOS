// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceDesktopName.hpp"
#include <service-eink/ServiceEinkDependencies.hpp>
#include <string>
#include <vector>

class ServiceDesktop;

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceDesktop>()
    {
        return sys::dependencies::getDependenciesTo<service::eink::ServiceEink>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceDesktop>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<ServiceDesktop>();
        dependencies.push_back(service::name::service_desktop);
        return dependencies;
    }

} // namespace sys::dependencies
