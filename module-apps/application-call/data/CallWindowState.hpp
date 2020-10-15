#pragma once

namespace gui::call
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

inline const char *c_str(gui::call::State state)
{
    switch (state) {
    case gui::call::State::IDLE:
        return "IDLE";
    case gui::call::State::INCOMING_CALL:
        return "INCOMING_CALL";
    case gui::call::State::OUTGOING_CALL:
        return "OUTGOING_CALL";
    case gui::call::State::CALL_IN_PROGRESS:
        return "CALL_IN_PROGRESS";
    case gui::call::State::CALL_ENDED:
        return "CALL_ENDED";
    }
    return "";
}
