// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include "LockBoxAlternatingSize.hpp"

#include <locks/data/LockStyle.hpp>
#include <locks/windows/LockInputWindow.hpp>

namespace gui
{
    void LockBoxAlternatingSize::buildLockBox(unsigned int pinSize)
    {
        buildPinLabels(0);
    }

    void LockBoxAlternatingSize::clear()
    {
        lockWindow->pinLabelsBox->erase();
        buildPinLabels(0);
    }

    void LockBoxAlternatingSize::popChar(unsigned int charNum)
    {
        buildPinLabels(charNum);
    }

    void LockBoxAlternatingSize::putChar(unsigned int charNum)
    {
        buildPinLabels(charNum + 1);
    }

    void LockBoxAlternatingSize::update(unsigned int inputFilled)
    {
        buildPinLabels(inputFilled);
    }

    void LockBoxAlternatingSize::buildPinLabels(unsigned int pinSize)
    {
        if (lockWindow->pinLabelsBox) {
            lockWindow->pinLabelsBox->erase();
        }

        auto itemBuilder = []() {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        lockWindow->buildPinLabels(itemBuilder, pinSize);
        lockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }
} // namespace gui
