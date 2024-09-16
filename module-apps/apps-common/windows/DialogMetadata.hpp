// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>
#include <functional>

namespace gui
{
    struct DialogMetadata
    {
        std::string title;
        UTF8 icon                    = "";
        UTF8 text                    = "No text";
        UTF8 iconText                = "";
        std::function<bool()> action = []() -> bool { return false; };

        DialogMetadata() = default;
    };
} // namespace gui
