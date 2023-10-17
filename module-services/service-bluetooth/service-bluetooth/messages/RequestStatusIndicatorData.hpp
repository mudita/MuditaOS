// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class RequestStatusIndicatorData : public BluetoothMessage
    {
      public:
        RequestStatusIndicatorData() = default;
    };
} // namespace message::bluetooth
