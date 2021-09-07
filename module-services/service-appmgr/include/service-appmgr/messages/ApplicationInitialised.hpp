// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class ApplicationInitialised : public BaseMessage
    {
      public:
        ApplicationInitialised(const ApplicationName &senderName,
                               StartupStatus _status,
                               StartInBackground _startInBackground);

        [[nodiscard]] auto getStatus() const noexcept -> StartupStatus;
        [[nodiscard]] auto isBackgroundApplication() const noexcept -> bool;

      private:
        StartupStatus status;
        StartInBackground startInBackground;
    };
} // namespace app::manager
