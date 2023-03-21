// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <appmgr/IdleHandler.hpp>
#include <service-appmgr/model/ApplicationManagerCommon.hpp>

namespace app::manager
{
    class ApplicationManager : public ApplicationManagerCommon, public IdleHandler
    {
      public:
        ApplicationManager(const ApplicationName &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationName &_rootApplicationName);

      protected:
        void handleStart(StartAllowedMessage *msg) override;
        auto handleAction(ActionEntry &action) -> ActionProcessStatus override;
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool override;

      private:
        sys::TimerHandle idleTimer;

        auto startApplication(ApplicationHandle &app) -> bool override;
        auto resolveHomeApplication() -> std::string override;
        auto registerMessageHandlers() -> void override;
    };
} // namespace app::manager
