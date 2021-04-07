// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLockBox.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class PukLockBox : public PinLockBox
    {
      public:
        explicit PukLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockBaseWindow *LockWindow;
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
