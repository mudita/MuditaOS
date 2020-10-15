#pragma once

#include <limits>

namespace gui
{
    namespace text
    {
        const unsigned int npos = std::numeric_limits<unsigned int>().max();
        const char newline      = '\n';
    }; // namespace text

    enum class ExpandMode
    {
        EXPAND_UP,
        EXPAND_DOWN,
        EXPAND_NONE // defult
    };

    enum class EditMode
    {
        BROWSE,
        EDIT,
        SCROLL,
    };

    enum class TextType
    {
        SINGLE_LINE = 1,
        MULTI_LINE
    };

    enum class InputBound
    {
        CANT_PROCESS = 0x100,
        NO_DATA      = 0x101,
        HIT_BOUND    = 0x110,

        CAN_ADD    = 0x1000,
        CAN_MOVE   = 0x10000,
        CAN_REMOVE = 0x100000
    };

    enum class Scroll
    {
        DOWN  = -1,
        UP    = 1,
        LEFT  = -1,
        RIGHT = 1,
        NONE  = 0
    };

} // namespace gui
