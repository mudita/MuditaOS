// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/PinLockBox.hpp"

namespace gui
{
    class PinLockWindow;
}

namespace gui
{
    class SimLockBox : public PinLockBox
    {
      public:
        SimLockBox(PinLockWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockWindow *LockWindow;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;
        void clear() final;

        void setVisibleStateEnterPin(EnterPasscodeType type) final;
        void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) final;
        void setVisibleStateBlocked() final;

        void buildLockBox(unsigned int pinSize) final;
        void buildPinLabels(unsigned int pinSize);
        void rebuildPinLabels(unsigned int pinSize);
    };
} // namespace gui
