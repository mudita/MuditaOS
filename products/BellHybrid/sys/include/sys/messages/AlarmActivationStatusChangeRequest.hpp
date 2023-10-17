// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    enum class AlarmActivationStatus
    {
        ACTIVATED,
        DEACTIVATED
    };

    class AlarmActivationStatusChangeRequest : public sys::DataMessage
    {
      public:
        explicit AlarmActivationStatusChangeRequest(AlarmActivationStatus status)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), status{status}
        {}

        [[nodiscard]] auto getStatus() const noexcept -> AlarmActivationStatus
        {
            return status;
        }

      private:
        const AlarmActivationStatus status;
    };
} // namespace sys
