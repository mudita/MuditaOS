// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LockBox.hpp"

#include <locks/windows/LockInputWindow.hpp>
#include <Image.hpp>

namespace gui
{
    class LockBoxConstantSize : public LockBox
    {
      public:
        void buildLockBox(unsigned int pinSize) override;
        void clear() final;
        void popChar(unsigned int charNum) final;
        void putChar(unsigned int charNum) final;
        void update(unsigned int inputFilled) final;

      protected:
        explicit LockBoxConstantSize(LockInputWindow *lockBaseWindow) : lockWindow(lockBaseWindow)
        {}

        void buildPinLabels(unsigned int pinSize);

      private:
        struct InputLabel : public HBox
        {
            InputLabel(Item *parent, uint32_t w, uint32_t h);
            void setVisibleState(bool isImageVisible);

            gui::Image *image = nullptr;
        };

        LockInputWindow *lockWindow;
        std::vector<InputLabel *> inputLabels;
    };
} // namespace gui
