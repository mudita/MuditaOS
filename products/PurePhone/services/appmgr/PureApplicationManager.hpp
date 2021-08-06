// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/model/ApplicationManager.hpp>

namespace app::manager
{
    class PureApplicationManager : public ApplicationManager
    {
      public:
        PureApplicationManager(const ApplicationName &serviceName,
                               std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                               const ApplicationName &_rootApplicationName)
            : ApplicationManager(std::move(serviceName), std::move(launchers), std::move(_rootApplicationName))
        {}
    };
} // namespace app::manager
