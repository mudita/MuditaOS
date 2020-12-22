// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace gui
{
    class PinLockBox
    {
      public:
        enum class EnterPasscodeType
        {
            ProvidePasscode,
            ProvideNewPasscode,
            ConfirmNewPasscode
        };

        enum class PasscodeErrorType
        {
            InvalidPasscode,
            NewPasscodeConfirmFailed,
            UnhandledError
        };

        virtual void popChar(unsigned int charNum) = 0;
        virtual void putChar(unsigned int charNum) = 0;
        virtual void clear()                       = 0;

        virtual void setVisibleStateEnterPin(EnterPasscodeType type)                       = 0;
        virtual void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) = 0;
        virtual void setVisibleStateBlocked()                                              = 0;

        virtual void buildLockBox(unsigned int pinSize) = 0;
        virtual ~PinLockBox()                           = default;
    };
} // namespace gui
