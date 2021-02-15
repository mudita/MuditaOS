// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "Error.hpp"
#include <functional>
#include <btstack_run_loop.h>
#include <string>

namespace bluetooth
{

    auto set_name(std::string &name) -> Error;
    namespace PAN
    {
        auto bnep_start() -> Error;
        auto bnep_setup() -> Error;
    } // namespace PAN
};    // namespace Bt
