// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Commands.hpp"
#include <at/Cmd.hpp>

namespace at
{
    struct Cmd;
    auto factory(AT at) -> const Cmd &;
} // namespace at
