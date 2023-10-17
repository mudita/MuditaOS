// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app::call
{
    enum class State
    {
        IDLE,
        INCOMING_CALL,
        OUTGOING_CALL,
        CALL_IN_PROGRESS,
        CALL_ENDED
    };
}

inline auto c_str(app::call::State state) -> const char *
{
    switch (state) {
    case app::call::State::IDLE:
        return "IDLE";
    case app::call::State::INCOMING_CALL:
        return "INCOMING_CALL";
    case app::call::State::OUTGOING_CALL:
        return "OUTGOING_CALL";
    case app::call::State::CALL_IN_PROGRESS:
        return "CALL_IN_PROGRESS";
    case app::call::State::CALL_ENDED:
        return "CALL_ENDED";
    }
    return "";
}
