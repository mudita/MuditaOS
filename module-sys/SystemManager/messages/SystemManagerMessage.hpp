// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace sys
{

    class SystemManagerMessage : public sys::DataMessage
    {
      public:
        SystemManagerMessage() : sys::DataMessage(MessageType::PMChangePowerMode){};
    };

} // namespace sys
