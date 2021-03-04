// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <deque>
#include <string>

#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>

namespace sdesktop::developerMode
{
    class ApplicationStackRequest : public Event
    {
      public:
        using Stack = app::manager::ApplicationManagerBase::ApplicationsStack;

        ApplicationStackRequest();

        void setResponse(const Stack &appStack);
    };

    class ApplicationStartRequest : public Event
    {
      public:
        ApplicationStartRequest(std::string appName);

        const std::string &getAppToStart() const;

        void appStartConfirmed();
        void appStartFailed();

      private:
        const std::string appToStart;
    };

    class ApplicationSwitchBackRequest : public Event
    {
      public:
        ApplicationSwitchBackRequest();

        void switchBackConfirmed();
        void switchBackFailed();
    };
} // namespace sdesktop::developerMode
