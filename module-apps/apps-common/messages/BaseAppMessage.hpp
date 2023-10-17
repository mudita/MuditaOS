// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MessageType.hpp"
#include "Service/Message.hpp"

namespace app
{
    class AppMessage : public sys::DataMessage
    {
      public:
        explicit AppMessage(MessageType messageType) : sys::DataMessage(messageType){};
        AppMessage() : sys::DataMessage(MessageType::AppMessage){};
    };
} // namespace app
