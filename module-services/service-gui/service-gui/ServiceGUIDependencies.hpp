// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-eink/ServiceEinkDependencies.hpp>
#include <service-gui/ServiceGUIName.hpp>
#include <string>
#include <vector>

namespace service::gui
{
    class ServiceGUI;
}
namespace sys::dependencies
{
    template <>
    inline std::vector<std::string> getDependenciesFor<service::gui::ServiceGUI>()
    {
        return sys::dependencies::getDependenciesTo<service::eink::ServiceEink>();
    }

    template <>
    inline std::vector<std::string> getDependenciesTo<service::gui::ServiceGUI>()
    {
        auto dependencies = sys::dependencies::getDependenciesFor<service::gui::ServiceGUI>();
        dependencies.push_back(service::name::gui);
        return dependencies;
    }
} // namespace sys::dependencies
