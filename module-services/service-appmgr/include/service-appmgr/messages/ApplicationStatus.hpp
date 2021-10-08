// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    /// Requests the application manager to check the status of the application.
    class ApplicationStatusRequest : public BaseMessage
    {
      public:
        ApplicationStatusRequest(const ApplicationName &senderName, ApplicationName applicationName);

        ApplicationName checkAppName;
    };

    class ApplicationStatusResponse : public sys::ResponseMessage
    {
      public:
        ApplicationStatusResponse(const ApplicationName &_applicationName, bool _isRunning);

        ApplicationName applicationName;
        bool isRunning;
    };
} // namespace app::manager
