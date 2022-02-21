// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#pragma once

namespace CellularCall
{
    class Call;
}

/// Guard to check if we should ring on cellular RING URC
/// This flow is when:
/// - We are in normal mode
/// - somebody calls
/// Without this check we could never ring on private numbers
bool callRingGuard(CellularCall::Call &call);

/// Guard to check if we should ring on cellular CLIP URC
/// This flow is when:
/// - We are in do not disturb mode
/// - somebody calls
/// - we have this somebody phone number via CLIP URC
/// It wont be ever called for private numbers
bool callClipGuard(CellularCall::Call &call);
