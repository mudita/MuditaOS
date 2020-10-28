// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLockBox.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class SimLockBox : public PinLockBox
    {
      public:
        SimLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockBaseWindow *LockWindow;
        void popChar(unsigned int charNum) override final;
        void putChar(unsigned int charNum) override final;

        void setVisibleStateEnterPin() override final;
        void setVisibleStateVerifiedPin() override final;
        void setVisibleStateInvalidPin() override final;
        void setVisibleStateBlocked() override final;

        void buildLockBox(unsigned int pinSize) override final;
        void buildPinLabels(unsigned int pinSize);
        void rebuildPinLabels(unsigned int pinSize);
    };
} // namespace gui
