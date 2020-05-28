#include "PhonebookListView.hpp"

namespace gui
{

    PhonebookListView::PhonebookListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : ListView(parent, x, y, w, h)
    {}

    void PhonebookListView::addLabelMarker(gui::PhonebookItem *item)
    {
        if (direction == style::listview::Direction::Bottom) {
            if (!(labelMark == (item)->getLabelMarker())) {

                labelMark = (item)->getLabelMarker();

                gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();
                phonebookLabel->setMarkerItem(labelMark);

                body->addWidget(phonebookLabel);
                addSpanItem();
            }
        }
        if (direction == style::listview::Direction::Top) {

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
                addSpanItem();
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
            if (direction == style::listview::Direction::Bottom) {
                addLabelMarker(reinterpret_cast<gui::PhonebookItem *>(item));
            }

            body->addWidget(item);

            // if added item is not visible -> page is full.
            if (item->visible != true) {

                // if page full and direction top remove last element and add floating label mark on top if there was no
                // one previously
                if (direction == style::listview::Direction::Top) {

                    if (previousItemIsLabel) {
                        break;
                    }
                    else {

                        body->eraseWidget(item);
                        body->eraseWidget(previousListItem);
                        body->eraseWidget(listSpanItem);

                        gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();

                        phonebookLabel->setMarkerItem(labelMark);

                        body->addWidget(phonebookLabel);
                        currentPageSize--;
                    }
                }
                break;
            }

            // if direction top add label mark after adding item
            if (direction == style::listview::Direction::Top) {
                previousItemIsLabel = false;
                addLabelMarker(reinterpret_cast<gui::PhonebookItem *>(item));
            }

            previousListItem = item;

            currentPageSize++;

            addSpanItem();
        }

        recalculateStartIndex();

        // Add element on top for first page purpose
        if (startIndex == 0 && direction == style::listview::Direction::Top) {
            gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();
            phonebookLabel->setMarkerItem(labelMark);
            body->addWidget(phonebookLabel);
        }
    }

} /* namespace gui */
