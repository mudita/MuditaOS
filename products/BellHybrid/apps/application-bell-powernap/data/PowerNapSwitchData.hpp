// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SwitchData.hpp>
namespace gui
{
    class PowerNapSwitchData : public SwitchData
    {
      public:
        PowerNapSwitchData()
        {
            ignoreCurrentWindowOnStack = true;
        }
    };

} // namespace gui
