// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "locks/data/LockStyle.hpp"
#include "locks/windows/PinLockBaseWindow.hpp"
#include "Lock.hpp"
#include "PhoneLockBaseBox.hpp"

namespace label_style = style::window::pin_lock::pin_label;

namespace gui
{
    void PhoneLockBaseBox::buildLockBox(unsigned int pinSize)
    {
        buildPinLabels(pinSize);
    }

    void PhoneLockBaseBox::clear()
    {
        for (unsigned i = 0; i < inputLabels.size(); i++) {
            popChar(i);
        }
    }

    void PhoneLockBaseBox::popChar(unsigned int charNum)
    {
        if (charNum < inputLabels.size()) {
            inputLabels[charNum]->setVisibleState(false);
        }
    }

    void PhoneLockBaseBox::putChar(unsigned int charNum)
    {
        if (charNum < inputLabels.size()) {
            inputLabels[charNum]->setVisibleState(true);
        }
    }

    void PhoneLockBaseBox::buildPinLabels(unsigned int pinSize)
    {
        constexpr auto pinLabelWidth = style::window::default_body_width;
        inputLabels.clear();

        if (pinSize == 0) {
            return;
        }

        unsigned int singleLabelWidth        = label_style::size;
        unsigned int maxNoMarginsLabelsWidth = pinLabelWidth - pinSize * 2 * label_style::margin;

        if (pinSize * singleLabelWidth > maxNoMarginsLabelsWidth) {
            singleLabelWidth = maxNoMarginsLabelsWidth / pinSize;
        }

        auto itemBuilder = [this, singleLabelWidth]() {
            auto label = new InputLabel(nullptr, singleLabelWidth, label_style::size);
            label->setEdges(RectangleEdge::Bottom);
            label->setMargins(Margins(label_style::margin, 0, label_style::margin, 0));
            inputLabels.push_back(label);
            return label;
        };

        lockWindow->buildPinLabels(
            itemBuilder, pinSize, style::window::default_left_margin, label_style::y, pinLabelWidth);
        lockWindow->pinLabelsBox->setEdges(RectangleEdge::None);
    }

    PhoneLockBaseBox::InputLabel::InputLabel(Item *parent, uint32_t w, uint32_t h) : HBox(parent, 0, 0, w, h)
    {}

    void PhoneLockBaseBox::InputLabel::setVisibleState(bool isImageVisible)
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
