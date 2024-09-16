// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "Profile.hpp"

namespace bluetooth
{
    bool Profile::isL2CapInitialized = false;
    bool Profile::isSdpInitialized   = false;

    void Profile::initSdp()
    {
        if (!isSdpInitialized) {
            sdp_init();
            isSdpInitialized = true;
        }
    }

    void Profile::initL2cap()
    {
        if (!isL2CapInitialized) {
            l2cap_init();
            isL2CapInitialized = true;
        }
    }
} // namespace bluetooth
