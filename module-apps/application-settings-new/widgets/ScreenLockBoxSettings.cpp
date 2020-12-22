// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBoxSettings.hpp"

namespace gui
{
    void ScreenLockBoxSettings::buildLockBox(unsigned int pinSize)
    {
        ScreenLockBox::buildPinLabels(pinSize);
    }
} // namespace gui
