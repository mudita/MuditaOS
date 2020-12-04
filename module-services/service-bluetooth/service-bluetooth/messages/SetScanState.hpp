// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Service/Message.hpp>
#include "service-bluetooth/BluetoothMessage.hpp"
#include <string>

namespace message::bluetooth
{
    class SetScanState : public BluetoothMessage
    {
      public:
        explicit SetScanState(bool state) : state(state)
        {}
        [[nodiscard]] auto getState() const noexcept -> bool
        {
            return state;
        }

      private:
        bool state;
    };
