// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/ScreenLockBox.hpp"

namespace gui
{
    class ScreenLockBoxSettings : public ScreenLockBox
    {
      public:
        ScreenLockBoxSettings(PinLockBaseWindow *LockBaseWindow) : ScreenLockBox(LockBaseWindow)
        {}

      private:
        void setVisibleStateEnterPin(EnterPasscodeType type) final;
        void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) final;
        void setVisibleStateBlocked() final
        {}
    };
} // namespace gui
