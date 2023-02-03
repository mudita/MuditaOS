// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <bsp/lpm/RT1051LPMCommon.hpp>

namespace bsp
{
    class RT1051LPM : public RT1051LPMCommon
    {
      public:
        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void SwitchToRegularModeLDO() final;
        void SwitchToLowPowerModeLDO() final;
    };

} // namespace bsp
