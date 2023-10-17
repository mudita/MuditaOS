// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationInitialised.hpp"

namespace app::manager
{
    ApplicationInitialised::ApplicationInitialised(const ApplicationName &senderName,
                                                   StartupStatus _status,
                                                   StartInBackground _startInBackground)
        : BaseMessage(MessageType::APMInit, senderName), status{_status}, startInBackground{_startInBackground}
    {}

    [[nodiscard]] auto ApplicationInitialised::getStatus() const noexcept -> StartupStatus
    {
        return status;
    }

    [[nodiscard]] auto ApplicationInitialised::isBackgroundApplication() const noexcept -> bool
    {
        return startInBackground.value;
    }
} // namespace app::manager
