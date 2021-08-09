// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/model/ApplicationManager.hpp>

namespace app::manager
{
    class BellApplicationManager : public ApplicationManager
    {
      public:
        BellApplicationManager(const ApplicationName &serviceName,
                               std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                               const ApplicationName &_rootApplicationName)
            : ApplicationManager(serviceName, std::move(launchers), _rootApplicationName)
        {}

      private:
        auto startApplication(ApplicationHandle &app) -> bool override;
        auto resolveHomeApplication() -> std::string override;
    };
} // namespace app::manager
