#include "PhonebookListView.hpp"

namespace gui
{

    PhonebookListView::PhonebookListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : ListView(parent, x, y, w, h)
    {}

    void PhonebookListView::addLabelMarker(gui::PhonebookItem *item)
    {
        if (!(labelMark == (item)->getLabelMarker())) {

            labelMark = (item)->getLabelMarker();

            gui::PhonebookItem *phonebookLabel = new gui::PhonebookItem();
            phonebookLabel->setMarkerItem(labelMark);

            body->addWidget(phonebookLabel);
        }
    }

    void PhonebookListView::addItemsOnPage()
    {
        auto itemsOnPage = 0;

        ListItem *item = nullptr;
        labelMark      = "";

        while ((item = provider->getItem(itemsOnPage)) != nullptr) {

            addLabelMarker(reinterpret_cast<gui::PhonebookItem *>(item));

            body->addWidget(item);

            if (item->visible != true) {
                currentPageSize = itemsOnPage;
                break;
            }

            itemsOnPage++;

            listSpanItem = new Span(Axis::Y, itemSpanSize);
            body->addWidget(listSpanItem);
        }

        if (currentPageSize == 0)
            currentPageSize = itemsOnPage;
    }

} /* namespace gui */
