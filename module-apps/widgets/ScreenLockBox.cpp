// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScreenLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18n/i18n.hpp>

namespace label_style = style::window::pin_lock::pin_label;

namespace gui
{
    ScreenLockBox::PinLabel::PinLabel(Item *parent, uint32_t w, uint32_t h) : HBox(parent, 0, 0, w, h)
    {}
    void ScreenLockBox::PinLabel::setVisibleState(bool isImageVisible)
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

    void ScreenLockBox::popChar(unsigned int charNum)
    {
        if (charNum < pinLabels.size()) {
            pinLabels[charNum]->setVisibleState(false);
        }
    }
    void ScreenLockBox::putChar(unsigned int charNum)
    {
        if (charNum < pinLabels.size()) {
            pinLabels[charNum]->setVisibleState(true);
        }
    }
    void ScreenLockBox::buildPinLabels(unsigned int pinSize)
    {
        constexpr auto pinLabelWidth = style::window::default_body_width;
        pinLabels.clear();

        if (pinSize == 0) {
            return;
        }

        unsigned int singleLabelWidth        = label_style::size;
        unsigned int maxNoMarginsLabelsWidth = pinLabelWidth - pinSize * 2 * label_style::margin;

        if (pinSize * singleLabelWidth > maxNoMarginsLabelsWidth) {
            singleLabelWidth = maxNoMarginsLabelsWidth / pinSize;
        }

        auto itemBuilder = [this, singleLabelWidth]() {
            auto label = new PinLabel(nullptr, singleLabelWidth, label_style::size);
            label->setEdges(RectangleEdge::Bottom);
            label->setMargins(Margins(label_style::margin, 0, label_style::margin, 0));
            pinLabels.push_back(label);
            return label;
        };

        lockBaseWindow->buildPinLabels(
            itemBuilder, pinSize, style::window::default_left_margin, label_style::y, pinLabelWidth);
        lockBaseWindow->pinLabelsBox->setEdges(RectangleEdge::None);
    }

    void ScreenLockBox::clear()
    {
        for (unsigned i = 0; i < pinLabels.size(); i++) {
            popChar(i);
        }
    }
} // namespace gui
