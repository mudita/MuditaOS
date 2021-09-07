// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include "LockBoxConstantSize.hpp"

#include <locks/data/LockStyle.hpp>
#include <locks/windows/LockInputWindow.hpp>

namespace gui
{
    void LockBoxConstantSize::buildLockBox(unsigned int pinSize)
    {
        buildPinLabels(pinSize);
    }

    void LockBoxConstantSize::clear()
    {
        for (unsigned i = 0; i < inputLabels.size(); i++) {
            popChar(i);
        }
    }

    void LockBoxConstantSize::popChar(unsigned int charNum)
    {
        if (charNum < inputLabels.size()) {
            inputLabels[charNum]->setVisibleState(false);
        }
    }

    void LockBoxConstantSize::putChar(unsigned int charNum)
    {
        if (charNum < inputLabels.size()) {
            inputLabels[charNum]->setVisibleState(true);
        }
    }

    void LockBoxConstantSize::update(unsigned int inputFilled)
    {
        if (inputFilled < inputLabels.size()) {
            for (unsigned int i = 0; i < inputFilled; i++) {
                inputLabels[i]->setVisibleState(true);
            }
        }
    }

    void LockBoxConstantSize::buildPinLabels(unsigned int pinSize)
    {
        using namespace style::window::lock_input;

        inputLabels.clear();

        if (pinSize == 0) {
            return;
        }

        unsigned int singleLabelWidth        = input_box::h;
        unsigned int maxNoMarginsLabelsWidth = input_box::w - pinSize * 2 * input_box::label_margin;

        if (pinSize * singleLabelWidth > maxNoMarginsLabelsWidth) {
            singleLabelWidth = maxNoMarginsLabelsWidth / pinSize;
        }

        auto itemBuilder = [this, singleLabelWidth]() {
            auto label = new InputLabel(nullptr, singleLabelWidth, input_box::h);
            label->setEdges(RectangleEdge::Bottom);
            label->setMargins(Margins(input_box::label_margin, 0, input_box::label_margin, 0));
            inputLabels.push_back(label);
            return label;
        };

        lockWindow->buildPinLabels(itemBuilder, pinSize);
    }

    LockBoxConstantSize::InputLabel::InputLabel(Item *parent, uint32_t w, uint32_t h) : HBox(parent, 0, 0, w, h)
    {}

    void LockBoxConstantSize::InputLabel::setVisibleState(bool isImageVisible)
    {
        if (isImageVisible && image == nullptr) {
            image = new gui::Image("dot_12px_hard_alpha_W_G");
            image->setVisible(true);
            image->activeItem = false;
            addWidget(image);
        }
        else if (!isImageVisible && image != nullptr) {
            erase(image);
            image = nullptr;
        }
    }
} // namespace gui
