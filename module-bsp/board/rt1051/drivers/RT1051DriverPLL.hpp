// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERPLL_HPP
#define PUREPHONE_RT1051DRIVERPLL_HPP

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

#endif // PUREPHONE_RT1051DRIVERPLL_HPP
