// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PinLockBox.hpp"
#include "BoxLayout.hpp"
#include "Image.hpp"

namespace gui
{
    class PinLockBaseWindow;
}

namespace gui
{
    class ScreenLockBox : public PinLockBox
    {
      public:
        ScreenLockBox(PinLockBaseWindow *LockBaseWindow) : lockBaseWindow(LockBaseWindow)
        {}
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;
        void clear() final;
        void setVisibleStateEnterPin(EnterPasscodeType type) override
        {}
        virtual void setVisibleStateInvalidPin(PasscodeErrorType type, unsigned int value) override
        {}
        void setVisibleStateBlocked() override
        {}

      protected:
        struct PinLabel : public HBox
        {
            gui::Image *image = nullptr;
            PinLabel(Item *parent, uint32_t w, uint32_t h);
            void setVisibleState(bool isImageVisible);
        };

        std::vector<PinLabel *> pinLabels;

        PinLockBaseWindow *lockBaseWindow;

        void buildPinLabels(unsigned int pinSize);
    };
} // namespace gui
