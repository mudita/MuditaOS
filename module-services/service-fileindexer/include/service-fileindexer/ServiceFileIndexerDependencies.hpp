// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/ServiceDBDependencies.hpp>
#include <service-fileindexer/ServiceFileIndexerName.hpp>
#include <string>
#include <vector>

namespace service
{
    class ServiceFileIndexer;
}

namespace sys::dependencies
{

    template <>
    inline std::vector<std::string> getDependenciesFor<service::ServiceFileIndexer>()
    {
        return sys::dependencies::getDependenciesTo<ServiceDB>();
    }
    template <>
    inline std::vector<std::string> getDependenciesTo<service::ServiceFileIndexer>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<service::ServiceFileIndexer>();
        dependencies.push_back(service::name::file_indexer);
        return dependencies;
    }
} // namespace sys::dependencies
