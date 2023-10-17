// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>

namespace gui
{
    namespace text
    {
        constexpr auto npos                     = std::numeric_limits<unsigned int>::max();
        const char newline                      = '\n';
        const unsigned int word_detection_range = 10;
    }; // namespace text

    enum class TextLimitType
    {
        MaxSize,
        MaxLines,
        MaxSignsCount
    };

    struct TextLimit
    {
        TextLimitType limitType;
        unsigned int limitValue;
    };

    enum class CursorStartPosition
    {
        DocumentBegin,
        Offset,
        DocumentEnd
    };

    enum class LinesDrawStop
    {
        None,
        OutOfText,
        OutOfSpace
    };

    enum class ExpandMode
    {
        Up,
        Down,
        None // default
    };

    enum class EditMode
    {
        Print,  /// <- Text in only printed, no navigation, no scrolling, no cursor visible.
        Browse, /// <- Text is printed with navigation enabled, without edit, scrolling and cursor not visible.
        Scroll, /// <- Text is printed with navigation and scroll enabled, without edit and cursor not visible.
        Edit,   /// <- Text is printed with navigation, scroll, cursor visible and edit.
    };

    enum class TextType
    {
        SingleLine,
        MultiLine
    };

    enum class TextEllipsis
    {
        Left,
        Right,
        Both,
        None
    };

    enum class AdditionBound
    {
        CanAddAll,
        CanAddPart,
        CantAdd
    };

    enum class AdditionType
    {
        perChar,
        perBlock
    };

} // namespace gui
