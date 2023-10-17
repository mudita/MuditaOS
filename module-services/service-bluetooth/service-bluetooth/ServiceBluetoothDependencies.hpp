// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-audio/ServiceAudioDependencies.hpp>
#include <service-bluetooth/ServiceBluetoothName.hpp>
#include <string>
#include <vector>

class ServiceBluetooth;

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<ServiceBluetooth>()
    {
        return sys::dependencies::getDependenciesTo<ServiceAudio>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<ServiceBluetooth>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<ServiceBluetooth>();
        dependencies.push_back(service::name::bluetooth);
        return dependencies;
    }
} // namespace sys::dependencies
