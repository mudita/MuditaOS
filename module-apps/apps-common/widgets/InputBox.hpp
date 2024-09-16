// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <TextFixedSize.hpp>

namespace gui
{
    auto inputBox(gui::Item *parent, const std::string &header, const std::string &icon = std::string()) -> gui::Text *;
}; // namespace gui
