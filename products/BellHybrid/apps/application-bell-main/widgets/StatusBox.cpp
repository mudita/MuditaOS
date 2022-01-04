// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatusBox.hpp"

namespace gui
{
    void StatusBox::setItems(Item *leftItem, Item *centerItem, Item *rightItem)
    {
        removeWidget(this->leftItem);
        removeWidget(this->centerItem);
        removeWidget(this->rightItem);
        this->leftItem   = leftItem;
        this->centerItem = centerItem;
        this->rightItem  = rightItem;
        addWidget(this->leftItem);
        addWidget(this->centerItem);
        addWidget(this->rightItem);
    } /* namespace gui */

    void StatusBox::resizeItems()
    {
        setAlignements();
        HBox::resizeItems();
    }
    auto StatusBox::handleRequestResize(const Item *child, Length request_w, Length request_h) -> Size
    {
        setAlignements();
        return HBox::handleRequestResize(child, request_w, request_h);
    }

    void StatusBox::setAlignements()
    {
        auto left   = (leftItem && leftItem->visible);
        auto center = (centerItem && centerItem->visible);
        auto right  = (rightItem && rightItem->visible);

        if (left) {
            if (center || right) {
                leftItem->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
            }
            else {
                leftItem->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            }
        }
        if (right) {
            if (center || left) {
                rightItem->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
            }
            else {
                rightItem->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            }
        }
        if (center) {
            if (left && !right) {
                centerItem->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
            }
            else if (right && !left) {
                centerItem->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
            }
            else {
                centerItem->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
            }
        }
    }
} // namespace gui
