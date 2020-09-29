#pragma once

#include <cstdint>

namespace gui
{
    class PinLockBox
    {
      public:
        virtual void popChar(uint32_t charNum) = 0;
        virtual void putChar(uint32_t charNum) = 0;

        virtual void setVisibleStateEnterPin()    = 0;
        virtual void setVisibleStateVerifiedPin() = 0;
        virtual void setVisibleStateInvalidPin()  = 0;
        virtual void setVisibleStateBlocked()     = 0;

        virtual void buildLockBox(unsigned int pinSize) = 0;
        virtual ~PinLockBox()                           = default;
    };
} // namespace gui
