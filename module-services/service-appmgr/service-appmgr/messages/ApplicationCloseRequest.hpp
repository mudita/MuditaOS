// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class ApplicationCloseRequest : public BaseMessage
    {
      public:
        ApplicationCloseRequest(const ApplicationName &senderName, ApplicationName application);

        [[nodiscard]] auto getApplication() const noexcept -> const ApplicationName &;

      private:
        ApplicationName application;
    };
} // namespace app::manager
