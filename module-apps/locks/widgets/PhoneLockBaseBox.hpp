// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "LockBox.hpp"
#include "locks/windows/LockWindow.hpp"

namespace gui
{
    class PhoneLockBaseBox : public LockBox
    {
      public:
        explicit PhoneLockBaseBox(LockWindow *lockBaseWindow) : lockWindow(lockBaseWindow)
        {}

        void buildLockBox(unsigned int pinSize) override;
        void clear() final;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;

      private:
        void buildPinLabels(unsigned int pinSize);
        void setVisibleStateBlocked() override
        {}
        void setVisibleStateInputRequired(InputActionType type) override
        {}
        void setVisibleStateInputInvalid(InputErrorType type, unsigned int value) override
        {}

        struct InputLabel : public HBox
        {
            InputLabel(Item *parent, uint32_t w, uint32_t h);
            void setVisibleState(bool isImageVisible);

            gui::Image *image = nullptr;
        };

        LockWindow *lockWindow;
        std::vector<InputLabel *> inputLabels;
    };
} // namespace gui
