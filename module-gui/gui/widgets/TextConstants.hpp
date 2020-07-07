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
} // namespace gui
