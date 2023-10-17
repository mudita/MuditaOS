// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string_view>

namespace Log
{
    static constexpr std::string_view CONSOLE_ESCAPE_NO_COLOR         = "";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_RESET      = "\x1b[0m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_RED        = "\x1b[31m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_GREEN      = "\x1b[32m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_YELLOW     = "\x1b[33m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_MAGENTA    = "\x1b[35m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_CYAN       = "\x1b[36m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_LIGHT_GRAY = "\x1b[90m";
    static constexpr std::string_view CONSOLE_ESCAPE_COLOR_LIGHT_BLUE = "\x1b[94m";

    static constexpr std::string_view levelColorsOn[] = {CONSOLE_ESCAPE_COLOR_LIGHT_BLUE,
                                                         CONSOLE_ESCAPE_COLOR_CYAN,
                                                         CONSOLE_ESCAPE_COLOR_GREEN,
                                                         CONSOLE_ESCAPE_COLOR_YELLOW,
                                                         CONSOLE_ESCAPE_COLOR_RED,
                                                         CONSOLE_ESCAPE_COLOR_MAGENTA};

    static constexpr std::string_view levelColorsOff[] = {CONSOLE_ESCAPE_NO_COLOR,
                                                          CONSOLE_ESCAPE_NO_COLOR,
                                                          CONSOLE_ESCAPE_NO_COLOR,
                                                          CONSOLE_ESCAPE_NO_COLOR,
                                                          CONSOLE_ESCAPE_NO_COLOR,
                                                          CONSOLE_ESCAPE_NO_COLOR};

    struct LogColors
    {
        const std::string_view *levelColors;
        const std::string_view serviceNameColor;
        const std::string_view callerInfoColor;
        const std::string_view resetColor;
    };

    static constexpr LogColors logColorsOn = {
        levelColorsOn,
        CONSOLE_ESCAPE_COLOR_MAGENTA,
        CONSOLE_ESCAPE_COLOR_LIGHT_GRAY,
        CONSOLE_ESCAPE_COLOR_RESET,
    };

    static constexpr LogColors logColorsOff = {
        levelColorsOff, CONSOLE_ESCAPE_NO_COLOR, CONSOLE_ESCAPE_NO_COLOR, CONSOLE_ESCAPE_NO_COLOR};
} // namespace Log
