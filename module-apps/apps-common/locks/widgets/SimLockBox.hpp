// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LockBoxAlternatingSize.hpp"
#include <locks/data/LockData.hpp>

namespace gui
{
    class LockInputWindow;
}

namespace gui
{
    class SimLockBox : public LockBoxAlternatingSize
    {
      public:
        explicit SimLockBox(LockInputWindow *LockBaseWindow,
                            locks::SimInputTypeAction simLockInputTypeAction = locks::SimInputTypeAction::UnlockWithPin)
            : LockBoxAlternatingSize(LockBaseWindow), LockWindow(LockBaseWindow)
        {
            applyLockActionText(simLockInputTypeAction);
        }

      private:
        void buildLockBox(unsigned int pinSize) final;
        void setVisibleStateInputRequired(InputActionType type) final;
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) final;
        void setVisibleStateBlocked(const std::string &formattedTime) final;
        void setVisibleStateError(unsigned int errorCode) final;
        void applyLockActionText(locks::SimInputTypeAction simLockInputTypeAction);

        LockInputWindow *LockWindow;
        std::string textForInputRequired;
        std::string textForInvalidInput;
        std::string textForInvalidInputLastAttempt;
        std::string textForInvalidInputLastAttemptWarning;
    };
} // namespace gui
