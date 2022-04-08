// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallRingGuard.hpp"
#include "call/CellularCall.hpp"

bool callRingGuard(CellularCall::Call &call)
{
    return call.mode == sys::phone_modes::PhoneMode::Connected;
}

bool callClipGuard(CellularCall::Call &call)
{
    return call.mode == sys::phone_modes::PhoneMode::DoNotDisturb && call.operations.areCallsFromFavouritesEnabled() &&
           call.operations.isNumberInFavourites();
}

bool callDNDGuard(CellularCall::Call &call)
{
    return call.mode == sys::phone_modes::PhoneMode::DoNotDisturb;
}
