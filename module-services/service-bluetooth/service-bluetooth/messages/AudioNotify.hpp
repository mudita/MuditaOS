// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class AudioPause : public BluetoothMessage
    {};

    class AudioStart : public BluetoothMessage
    {};

} // namespace message::bluetooth
