// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>
#include <PhoneModes/PhoneMode.hpp>
#include <service-cellular/CellularMessage.hpp>

namespace call::event
{
    struct Answer
    {};
    struct StartCall
    {
        CallType type;
        const utils::PhoneNumber::View number;
    };
    struct RING
    {};

    /// event signaling that we should not delay sending RING to UI any further
    struct RingTimeout
    {};
    struct CLIP
    {
        const utils::PhoneNumber::View number;
    };
    struct AudioRequest
    {
        cellular::CallAudioEventRequest::EventType event;
    };

    struct ModeChange
    {
        sys::phone_modes::PhoneMode mode;
    };

    struct TetheringChange
    {
        sys::phone_modes::Tethering tethering;
    };

    struct OngoingTimer
    {};

    struct Ended
    {};

    struct Reject
    {};

} // namespace call::event
