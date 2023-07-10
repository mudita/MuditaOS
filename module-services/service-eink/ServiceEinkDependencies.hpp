// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/ServiceEventManagerDependencies.hpp>
#include <service-eink/ServiceEinkName.hpp>
#include <string>
#include <vector>

namespace service::eink
{
    class ServiceEink;
}

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<service::eink::ServiceEink>()
    {
        return sys::dependencies::getDependenciesTo<EventManager>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<service::eink::ServiceEink>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<service::eink::ServiceEink>();
        dependencies.push_back(service::name::eink);
        return dependencies;
    }
} // namespace sys::dependencies
