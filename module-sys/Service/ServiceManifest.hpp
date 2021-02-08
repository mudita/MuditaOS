// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace sys
{
    class Service; // Forward declaration

    struct ServiceManifest
    {
        using ServiceName                    = std::string;
        using Timeout                        = std::chrono::milliseconds;
        static constexpr auto DefaultTimeout = Timeout{5000};

        std::vector<ServiceName> dependencies;
        ServiceName name;
        Timeout timeout = DefaultTimeout;
    };

    /// Type traits pattern used to enforce user-defined types to implement "GetManifest" function.
    template <class T> struct ManifestTraits;

    template <class, class = void> struct HasManifest : std::false_type
    {};

    /// Checks whether T implements "GetManifest" static method.
    /// Provides the member constant "value" that is equal to true if T implements "GetManifest" static method.
    /// Otherwise, "value" is equal to false.
    template <class T>
    struct HasManifest<T, std::void_t<decltype(&ManifestTraits<T>::GetManifest)>>
        : std::is_same<ServiceManifest, decltype(ManifestTraits<T>::GetManifest())>
    {};

    /// Retrieves the manifest of T, if T implements ManifestTraits.
    /// Otherwise, reports an error during compile time.
    template <class T, std::enable_if_t<HasManifest<T>::value, int> = 0> auto ManifestOf() -> ServiceManifest
    {
        return ManifestTraits<T>::GetManifest();
    }
} // namespace sys
