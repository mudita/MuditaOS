// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Message.hpp>
#include "service-bluetooth/BluetoothMessage.hpp"
#include <string>

namespace message::bluetooth
{

    class RequestStatus : public BluetoothMessage
    {};

    class ResponseStatus : public BluetoothMessage
    {
      public:
        explicit ResponseStatus(BluetoothStatus status) : status(std::move(status))
        {}
        [[nodiscard]] auto getStatus() const noexcept -> BluetoothStatus
        {
            return status;
        }

      private:
        BluetoothStatus status;
    };
} // namespace message::bluetooth
