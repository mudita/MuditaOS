// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <string>

namespace gui
{
    class LockBox
    {
      public:
        enum class InputActionType
        {
            ProvideInput,
            ProvideNewInput,
            ConfirmNewInput
        };

        enum class InputErrorType
        {
            InvalidInput,
            NewInputConfirmFailed,
        };

        virtual void popChar(unsigned int charNum)    = 0;
        virtual void putChar(unsigned int charNum)    = 0;
        virtual void update(unsigned int inputFilled) = 0;
        virtual void clear()                          = 0;

        virtual void setVisibleStateInputRequired(InputActionType type)                   = 0;
        virtual void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) = 0;
        virtual void setVisibleStateBlocked(const std::string &formattedTime)             = 0;
        virtual void setVisibleStateError(unsigned int errorCode)                         = 0;

        virtual void buildLockBox(unsigned int inputSize) = 0;
        virtual ~LockBox()                                = default;
    };
} // namespace gui
