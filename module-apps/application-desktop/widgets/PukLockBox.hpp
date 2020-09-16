#pragma once

#include "PinLockBox.hpp"
#include "Label.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class PukLockBox : public PinLockBox
    {
      public:
        PukLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockBaseWindow *LockWindow;
        void popChar(uint32_t charNum) override;
        void putChar(uint32_t charNum) override;

        void setVisibleStateEnterPin() override;
        void setVisibleStateVerifiedPin() override;
        void setVisibleStateInvalidPin(uint32_t remainingAttempts) override;
        void setVisibleStatePhoneBlocked() override;

        void buildLockBox(unsigned int pinSize) override final;
        void buildPinLabels(uint32_t pinSize);
    };
} // namespace gui
