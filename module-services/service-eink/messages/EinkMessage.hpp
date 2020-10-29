// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

namespace seink
{

    /*
     * @brief Template for all messages that go to gui service
     */
    class EinkMessage : public sys::DataMessage
    {
      public:
        EinkMessage(MessageType messageType) : sys::DataMessage(messageType){};
        virtual ~EinkMessage(){};
    };

} /* namespace seink */
