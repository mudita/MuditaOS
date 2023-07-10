// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys::bluetooth
{
    enum class BluetoothMode
    {
        ConnectedVoice,
        ConnectedAudio,
        ConnectedBoth,
        Disabled,
        Enabled
    };
}
