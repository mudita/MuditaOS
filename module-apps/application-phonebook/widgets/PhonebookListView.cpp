// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookListView.hpp"

namespace gui
{
    PhonebookListView::PhonebookListView(Item *parent,
                                         std::uint32_t x,
                                         std::uint32_t y,
                                         std::uint32_t w,
                                         std::uint32_t h,
                                         std::shared_ptr<ListItemProvider> prov)
        : ListView(parent, x, y, w, h, std::move(prov))
    {}

    void PhonebookListView::addLabelMarker(gui::PhonebookItem *item)
    {
        switch (direction) {
        case listview::Direction::Bottom:
            if (labelMark != item->getLabelMarker()) {
                labelMark = item->getLabelMarker();
                body->addWidget(new PhonebookMarkItem(labelMark));
            }
            break;

        case listview::Direction::Top:
            if (currentPageSize == 0) {
                labelMark = item->getLabelMarker();
                break;
            }

            if (labelMark != item->getLabelMarker()) {
                const auto initialSlotsLeft  = getSlotsLeft();
                const auto previousLabelMark = labelMark;
                labelMark                    = item->getLabelMarker();

                body->removeWidget(item);
                body->addWidget(new PhonebookMarkItem(previousLabelMark));

                /* Add item to body even if it won't fit to avoid manual memory
                 * management for item, but apply correction to currentPageSize
                 * if it is not visible. */
                body->addWidget(item);
                if (initialSlotsLeft == 0) {
                    currentPageSize--;
                }

                previousItemIsLabel = true;
            }
            break;
        }
    }

    void PhonebookListView::addItemsOnPage()
    {
        currentPageSize            = 0;
        labelMark                  = "";

        ListItem *item             = nullptr;
        ListItem *previousListItem = nullptr;

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            /* If direction is top-to-bottom, add label mark before adding phonebook item. */
            if (direction == listview::Direction::Bottom) {
                addLabelMarker(dynamic_cast<gui::PhonebookItem *>(item));
            }

            /* Check if new item fits, if it does - add it, if not - handle label insertion
             * case for bottom-to-top navigation direction. */
            if (getSlotsLeft() > 0) {
                body->addWidget(item);
            }
            else {
                if (direction == listview::Direction::Top) {
                    if (previousItemIsLabel) {
                        break;
                    }
                    body->erase(previousListItem);
                    body->addWidget(new PhonebookMarkItem(labelMark));
                    currentPageSize--;
                }
                break;
            }

            /* If direction is bottom-to-top, add label mark after adding phonebook item. */
            if (direction == listview::Direction::Top) {
                previousItemIsLabel = false;
                addLabelMarker(dynamic_cast<gui::PhonebookItem *>(item));
            }

            previousListItem = item;
            currentPageSize++;
        }

        recalculateStartIndex();

        // Add element on top for first page purpose
        if (startIndex == 0 && direction == listview::Direction::Top) {
            body->addWidget(new PhonebookMarkItem(labelMark));
        }
    }

    std::size_t PhonebookListView::getSlotsLeft()
    {
        const auto singleSlotSpace = getProvider()->getMinimalItemSpaceRequired();
        return body->getPrimarySizeLeft() / singleSlotSpace;
    }
} /* namespace gui */
