// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>

namespace sevm
{
    enum class LatchStatus
    {
        PRESSED,
        RELEASED
    };

    class LatchStatusRequest : public sys::DataMessage
    {
      public:
        LatchStatusRequest() : sys::DataMessage(MessageType::LatchStateMessage)
        {}
    };

    class LatchStatusResponse : public sys::ResponseMessage
    {
        LatchStatus status;

      public:
        LatchStatusResponse(LatchStatus status) : status(status)
        {}

        [[nodiscard]] auto getStatus() const noexcept -> LatchStatus
        {
            return status;
        }
    };
} // namespace sevm
