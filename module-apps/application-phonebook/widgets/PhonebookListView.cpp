// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookListView.hpp"

namespace gui
{

    PhonebookListView::PhonebookListView(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov)
        : ListView(parent, x, y, w, h, prov)
    {}

    void PhonebookListView::addLabelMarker(gui::PhonebookItem *item)
    {
        if (direction == listview::Direction::Bottom) {
            if (!(labelMark == (item)->getLabelMarker())) {

                labelMark = (item)->getLabelMarker();

                gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();
                phonebookLabel->setMarkerItem(labelMark);

                body->addWidget(phonebookLabel);
            }
        }
        if (direction == listview::Direction::Top) {

            if (currentPageSize == 0) {
                labelMark = (item)->getLabelMarker();
                return;
            }
            else if (!(labelMark == (item)->getLabelMarker())) {

                previousLabelMark = labelMark;
                labelMark         = (item)->getLabelMarker();

                gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();
                phonebookLabel->setMarkerItem(previousLabelMark);

                body->removeWidget(item);
                body->addWidget(phonebookLabel);
                body->addWidget(item);

                previousItemIsLabel = true;
            }
        }
    }

    void PhonebookListView::addItemsOnPage()
    {
        currentPageSize            = 0;
        ListItem *item             = nullptr;
        ListItem *previousListItem = nullptr;
        labelMark                  = "";
        previousLabelMark          = "";

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            // if direction bot add label mark before adding item
            if (direction == listview::Direction::Bottom) {
                addLabelMarker(dynamic_cast<gui::PhonebookItem *>(item));
            }

            body->addWidget(item);

            // if added item is not visible -> page is full.
            if (item->visible != true) {

                // if page full and direction top remove last element and add floating label mark on top if there was no
                // one previously
                if (direction == listview::Direction::Top) {

                    if (previousItemIsLabel) {
                        break;
                    }
                    else {

                        body->erase(item);
                        body->erase(previousListItem);

                        gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();

                        phonebookLabel->setMarkerItem(labelMark);

                        body->addWidget(phonebookLabel);
                        currentPageSize--;
                    }
                }
                break;
            }

            // if direction top add label mark after adding item
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
            auto *phonebookLabel = new gui::PhonebookItem();
            phonebookLabel->setMarkerItem(labelMark);
            body->addWidget(phonebookLabel);
        }
    }

} /* namespace gui */
