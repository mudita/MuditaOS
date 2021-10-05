// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/model/ApplicationManagerCommon.hpp>

namespace app::manager
{
    constexpr auto idleReturnTimeout = std::chrono::milliseconds{30000};

    class ApplicationManager : public ApplicationManagerCommon
    {
      public:
        ApplicationManager(const ApplicationName &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationName &_rootApplicationName);

      private:
        sys::TimerHandle idleTimer;

        auto startApplication(ApplicationHandle &app) -> bool override;
        auto resolveHomeApplication() -> std::string override;
        void registerMessageHandlers() override;

        void handleIdleTimerRestart(sys::Message *request);
        void handleIdleTimerStop(sys::Message *request);
        void idleTimerCallback();
    };
} // namespace app::manager
