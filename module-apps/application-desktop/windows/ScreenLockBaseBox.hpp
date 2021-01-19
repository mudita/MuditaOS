// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "PinLockBox.hpp"
#include "LockWindow.hpp"

namespace gui
{
    class ScreenLockBaseBox : public PinLockBox
    {
      public:
        ScreenLockBaseBox(LockWindow *lockBaseWindow) : lockWindow(lockBaseWindow)
        {}

        void buildLockBox(unsigned int pinSize) override;
        void clear() final;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;

      private:
        void buildPinLabels(unsigned int pinSize);
        void setVisibleStateBlocked()
        {}
        void setVisibleStateEnterPin(EnterPasscodeType type)
        {}
        void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value)
        {}

        struct PinLabel : public HBox
        {
            PinLabel(Item *parent, uint32_t w, uint32_t h);
            void setVisibleState(bool isImageVisible);

            gui::Image *image = nullptr;
        };

        LockWindow *lockWindow;
        std::vector<PinLabel *> pinLabels;
    };
} // namespace gui
