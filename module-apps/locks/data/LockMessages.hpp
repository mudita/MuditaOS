// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace lock
{
    class UnlockPhone : public sys::DataMessage
    {
      public:
        explicit UnlockPhone() : DataMessage{MessageType::MessageTypeUninitialized}
        {}
    };

    class LockPhone : public sys::DataMessage
    {
      public:
        explicit LockPhone() : DataMessage{MessageType::MessageTypeUninitialized}
        {}
    };

    class ChangedSuccessfully : public sys::ResponseMessage
    {};

    class ChangeFailed : public sys::ResponseMessage
    {};
} // namespace lock
