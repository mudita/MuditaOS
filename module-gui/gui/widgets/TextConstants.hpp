#pragma once

namespace gui
{

    enum class LineEndType
    {
        EOT,           // end of source text
        BREAK,         // line had enter defined as a /r
        CONTINUE,      // line was broken due to not enough space to hold all characters
        CONTINUE_SPACE // line was broken on the space character because next word doesn't fit current line.
    };

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

    enum class NavigationBarrier
    {
        BARRIER_NONE  = 0x00,
        BARRIER_UP    = 0x01,
        BARRIER_DOWN  = 0x02,
        BARRIER_LEFT  = 0x04,
        BARRIER_RIGHT = 0x08,
        BARRIER_ALL   = 0x0F,
    };
} // namespace gui
