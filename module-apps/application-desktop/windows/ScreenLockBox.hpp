#pragma once

#include "PinLockBox.hpp"
#include "Label.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class ScreenLockBox : public PinLockBox
    {
      public:
        ScreenLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockBaseWindow *LockWindow;
        void popChar(uint32_t charNum) override final;
        void putChar(uint32_t charNum) override final;

        void setVisibleStateEnterPin() override final;
        void setVisibleStateVerifiedPin() override final;
        void setVisibleStateInvalidPin() override final;
        void setVisibleStateBlocked() override final;

        void buildLockBox(unsigned int pinSize) override final;
        void buildPinLabels(unsigned int pinSize);
    };
} // namespace gui
