// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "drivers/pll/DriverPLL.hpp"
#include <string>

namespace drivers
{
    class RT1051DriverPLL : public DriverPLL
    {
      public:
        RT1051DriverPLL(const PLLInstances &inst, const DriverPLLParams &params);
        ~RT1051DriverPLL();

      private:
        PLLInstances instance;
        std::string name;
    };
} // namespace drivers
