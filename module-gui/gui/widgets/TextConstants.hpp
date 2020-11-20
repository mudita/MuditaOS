// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>

namespace gui
{
    namespace text
    {
        const unsigned int npos = std::numeric_limits<unsigned int>().max();
        const char newline      = '\n';
    }; // namespace text

    enum class TextLimitType
    {
        MAX_SIZE,
        MAX_LINES,
        MAX_SIGNS_COUNT
    };

    struct TextLimit
    {
        TextLimitType limitType;
        unsigned int limitValue;
    };

    enum class ExpandMode
    {
        EXPAND_UP,
        EXPAND_DOWN,
        EXPAND_NONE // default
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
        CAN_ADD,
        CAN_ADD_PART,
        HIT_BOUND
    };

} // namespace gui
