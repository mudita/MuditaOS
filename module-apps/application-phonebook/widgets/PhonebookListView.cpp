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
                body->addWidget(item);
            }
        }
    }

    void PhonebookListView::addItemsOnPage()
    {
        currentPageSize = 0;
        //        auto itemsOnPage = 0;
        ListItem *item             = nullptr;
        ListItem *previousListItem = nullptr;
        labelMark                  = "";
        previousLabelMark          = "";

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            if (direction == style::listview::Direction::Bottom) {
                addLabelMarker(reinterpret_cast<gui::PhonebookItem *>(item));
            }

            body->addWidget(item);

            if (item->visible != true) {

                if (direction == style::listview::Direction::Top) {
                    body->removeWidget(item);
                    body->removeWidget(previousListItem);
                    gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();

                    if (!(previousLabelMark == labelMark)) {
                        phonebookLabel->setMarkerItem(labelMark);
                    }

                    body->addWidget(phonebookLabel);

                    currentPageSize--;
                }
                //                currentPageSize = itemsOnPage;
                break;
            }

            //            itemsOnPage++;

            if (direction == style::listview::Direction::Top) {
                addLabelMarker(reinterpret_cast<gui::PhonebookItem *>(item));
            }

            previousListItem = item;
            //            previousLabelMark = reinterpret_cast<gui::PhonebookItem *>(item)->getLabelMarker();
            currentPageSize++;

            LOG_DEBUG("Page id %d, Label Mark %s", currentPageSize, labelMark.c_str());
            LOG_DEBUG("Page id %d, Prev Mark %s", currentPageSize, previousLabelMark.c_str());

            listSpanItem = new Span(Axis::Y, itemSpanSize);
            body->addWidget(listSpanItem);
        }
    }

} /* namespace gui */
