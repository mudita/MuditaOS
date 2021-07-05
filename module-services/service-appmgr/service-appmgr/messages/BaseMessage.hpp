// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <MessageType.hpp>
#include <apps-common/Application.hpp>

namespace app::manager
{
    class BaseMessage : public sys::DataMessage
    {
      public:
        BaseMessage(MessageType type, ApplicationName sender);
        explicit BaseMessage(ApplicationName sender);
        BaseMessage() = default;

        [[nodiscard]] auto getSenderName() const noexcept -> const ApplicationName &;

      private:
        ApplicationName senderName;
    };
} // namespace app::manager
