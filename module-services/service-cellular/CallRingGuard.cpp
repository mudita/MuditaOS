// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallRingGuard.hpp"
#include "service-cellular/CellularCall.hpp"

bool callRingGuard(CellularCall::CellularCall &call)
{
    return call.mode == sys::phone_modes::PhoneMode::Connected ||
           (call.mode == sys::phone_modes::PhoneMode::DoNotDisturb && call.operations.areCallsFromFavouritesEnabled() &&
            call.operations.isNumberInFavourites());
}
