// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class AudioPause : public BluetoothMessage
    {};

    class AudioStart : public BluetoothMessage
    {};

} // namespace message::bluetooth
