// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Commands.hpp"
#include <at/Cmd.hpp>

namespace at
{
    struct Cmd;
    auto factory(AT at) -> const Cmd &;
} // namespace at
