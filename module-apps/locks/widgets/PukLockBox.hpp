// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LockBox.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class PukLockBox : public LockBox
    {
      public:
        explicit PukLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        PinLockBaseWindow *LockWindow;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;
        void clear() final;

        void setVisibleStateInputRequired(InputActionType type) final;
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) final;
        void setVisibleStateBlocked() final;

        void buildLockBox(unsigned int inputSize) final;
        void buildInputLabels(unsigned int inputSize);
        void rebuildInputLabels(unsigned int inputSize);
    };
} // namespace gui
