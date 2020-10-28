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
        ScreenLockBox(PinLockBaseWindow *LockBaseWindow) : LockWindow(LockBaseWindow)
        {}

      private:
        struct PinLabel : public HBox
        {
            gui::Image *image;
            PinLabel(Item *parent, const uint32_t &w, const uint32_t &h);
            void setVisibleState(bool isImageVisible);
        };

        std::vector<PinLabel *> pinLabels;
        PinLockBaseWindow *LockWindow;
        void popChar(unsigned int charNum) override final;
        void putChar(unsigned int charNum) override final;

        void setVisibleStateEnterPin() override final;
        void setVisibleStateVerifiedPin() override final;
        void setVisibleStateInvalidPin() override final;
        void setVisibleStateBlocked() override final;

        void buildLockBox(unsigned int pinSize) override final;
        void buildPinLabels(unsigned int pinSize);
        void clearPinLabels();
    };
} // namespace gui
