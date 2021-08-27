// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "board/clock_config.h"

namespace drivers
{
    inline constexpr uint32_t SemcMuxPLL2Clock{1};
    inline constexpr uint32_t SemcMuxPeripheralClock{0};

    class RT1051DriverPLL2
    {
      public:
        RT1051DriverPLL2() noexcept;
        ~RT1051DriverPLL2();

      private:
        [[nodiscard]] bool IsPLL2Enabled() const noexcept;
    };

} // namespace drivers
