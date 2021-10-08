// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Text.hpp>

namespace gui
{
    auto inputBox(gui::Item *parent, const std::string &header, const std::string &icon = std::string()) -> gui::Text *;
}; // namespace gui
