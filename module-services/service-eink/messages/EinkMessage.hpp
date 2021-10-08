// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace service::eink
{
    class EinkMessage : public sys::DataMessage
    {
      public:
        EinkMessage() : sys::DataMessage(MessageType::EinkMessage)
        {}
    };
} /* namespace seink */
