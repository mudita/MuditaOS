// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Error.hpp"

#include <string>

namespace bluetooth
{
    auto set_name(std::string &name) -> Error;
} // namespace bluetooth
