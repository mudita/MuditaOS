// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-audio/AudioServiceName.hpp"
#include <service-db/ServiceDBDependencies.hpp>
#include <string>
#include <vector>

class ServiceAudio;
namespace service
{
    class Audio;
}

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceAudio>()
    {
        return sys::dependencies::getDependenciesTo<ServiceDB>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceAudio>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<ServiceAudio>();
        dependencies.push_back(service::name::audio);
        return dependencies;
    }

    template <>
    inline std::vector<std::string> getDependenciesFor<service::Audio>()
    {
        return sys::dependencies::getDependenciesTo<ServiceDB>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<service::Audio>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<service::Audio>();
        dependencies.push_back(service::name::audio);
        return dependencies;
    }
} // namespace sys::dependencies
