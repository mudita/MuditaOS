// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/DuoHBox.hpp>

namespace gui
{
    void DuoHBox::setItems(Item *leftItem, Item *rightItem)
    {
        removeWidget(this->leftItem);
        removeWidget(this->rightItem);
        this->leftItem  = leftItem;
        this->rightItem = rightItem;
        addWidget(this->leftItem);
        addWidget(this->rightItem);
    }

    void DuoHBox::resizeItems()
    {
        setAlignements();
        HBox::resizeItems();
    }
    auto DuoHBox::handleRequestResize(const Item *child, Length request_w, Length request_h) -> Size
    {
        setAlignements();
        return HBox::handleRequestResize(child, request_w, request_h);
    }

    void DuoHBox::setAlignements()
    {
        auto left  = (leftItem && leftItem->visible);
        auto right = (rightItem && rightItem->visible);

        if (left ^ right) {
            auto soloItem = left ? leftItem : rightItem;
            soloItem->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        }
        else if (left && right) {
            leftItem->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
            rightItem->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        }
    }
} // namespace gui
