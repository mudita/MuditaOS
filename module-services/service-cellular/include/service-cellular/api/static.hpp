// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common.hpp"

#include <string>

namespace cellular::api
{
    SimSlot currentSimSlot();
    SimState currentSimState();
    TrayState trayState();
    std::string ownNumber();
} // namespace cellular::api
