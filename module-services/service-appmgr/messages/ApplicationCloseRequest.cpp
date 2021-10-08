// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCloseRequest.hpp"

namespace app::manager
{
    ApplicationCloseRequest::ApplicationCloseRequest(const ApplicationName &senderName, ApplicationName application)
        : BaseMessage(MessageType::APMDelayedClose, senderName), application{std::move(application)}
    {}

    [[nodiscard]] auto ApplicationCloseRequest::getApplication() const noexcept -> const ApplicationName &
    {
        return application;
    }
} // namespace app::manager
