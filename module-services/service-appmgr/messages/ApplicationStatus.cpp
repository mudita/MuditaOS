// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationStatus.hpp"

namespace app::manager
{
    ApplicationStatusRequest::ApplicationStatusRequest(const ApplicationName &senderName,
                                                       ApplicationName applicationName)
        : BaseMessage(MessageType::APMCheckAppRunning, senderName), checkAppName(std::move(applicationName))
    {}

    ApplicationStatusResponse::ApplicationStatusResponse(const ApplicationName &_applicationName, bool _isRunning)
        : sys::ResponseMessage(sys::ReturnCodes::Success, MessageType::APMCheckAppRunning),
          applicationName{std::move(_applicationName)}, isRunning{_isRunning}
    {}
} // namespace app::manager
