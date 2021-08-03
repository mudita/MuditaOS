// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LockBox.hpp"

#include <locks/windows/LockInputWindow.hpp>
#include <Image.hpp>

namespace gui
{
    class LockBoxAlternatingSize : public LockBox
    {
      public:
        void buildLockBox(unsigned int pinSize) override;
        void clear() final;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;
        void update(unsigned int inputFilled) final;

      protected:
        explicit LockBoxAlternatingSize(LockInputWindow *lockBaseWindow) : lockWindow(lockBaseWindow)
        {}

        void buildPinLabels(unsigned int pinSize);

      private:
        LockInputWindow *lockWindow;
    };
} // namespace gui
