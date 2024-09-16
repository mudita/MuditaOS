// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "TextFormat.hpp"
#include <RawFont.hpp>

namespace gui
{
    auto TextFormat::str() const -> std::string
    {
        std::string ret = font->getName();
        ret += " color: ";
        ret += std::to_string(color.intensity);
        return ret;
    }
} // namespace gui
