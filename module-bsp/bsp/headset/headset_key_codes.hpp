// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdint>

namespace bsp {
 namespace headset {

    enum class HeadsetState : bool
    {
        NoChange = false,
        Changed
    };

    enum class KeyCode : uint8_t
    {
        Error = 0,
        Key1,
        Key2,
        Key3,
        Key4
    };

    enum class KeyEvent : uint8_t
    {
        KeyReleased = 0,
        KeyPressed,
        Error
    };
 }
}
