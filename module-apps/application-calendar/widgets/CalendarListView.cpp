#include "CalendarListView.hpp"

namespace gui
{

    CalendarListView::CalendarListView(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov)
        : ListView(parent, x, y, w, h, prov)
    {}

    void CalendarListView::addLabelMarker(gui::CalendarAllEventsItem *item)
    {
        if (direction == style::listview::Direction::Bottom) {
            if (!(labelMark == (item)->getLabelMarker())) {

                labelMark = (item)->getLabelMarker();

                gui::CalendarAllEventsItem *calendarAllEventsItem = new gui::CalendarAllEventsItem();
                calendarAllEventsItem->setMarkerItem(labelMark);

                body->addWidget(calendarAllEventsItem);
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

                gui::CalendarAllEventsItem *calendarAllEventsItem = new gui::CalendarAllEventsItem();
                calendarAllEventsItem->setMarkerItem(previousLabelMark);

                body->removeWidget(item);
                body->addWidget(calendarAllEventsItem);
                body->addWidget(item);

                previousItemIsLabel = true;
            }
        }
    }

    void CalendarListView::addItemsOnPage()
    {
        currentPageSize            = 0;
        ListItem *item             = nullptr;
        ListItem *previousListItem = nullptr;
        labelMark                  = "";
        previousLabelMark          = "";

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            // if direction bot add label mark before adding item
            if (direction == style::listview::Direction::Bottom) {
                addLabelMarker(dynamic_cast<gui::CalendarAllEventsItem *>(item));
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

                        body->erase(item);
                        body->erase(previousListItem);

                        gui::CalendarAllEventsItem *calendarAllEventsItem = new gui::CalendarAllEventsItem();

                        calendarAllEventsItem->setMarkerItem(labelMark);

                        body->addWidget(calendarAllEventsItem);
                        currentPageSize--;
                    }
                }
                break;
            }

            // if direction top add label mark after adding item
            if (direction == style::listview::Direction::Top) {
                previousItemIsLabel = false;
                addLabelMarker(dynamic_cast<gui::CalendarAllEventsItem *>(item));
            }

            previousListItem = item;

            currentPageSize++;
        }

        recalculateStartIndex();

        // Add element on top for first page purpose
        if (startIndex == 0 && direction == style::listview::Direction::Top) {
            gui::CalendarAllEventsItem *calendarAllEventsItem = new gui::CalendarAllEventsItem();
            calendarAllEventsItem->setMarkerItem(labelMark);
            body->addWidget(calendarAllEventsItem);
        }
    }

} /* namespace gui */
