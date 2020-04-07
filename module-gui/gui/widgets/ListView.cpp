/*
 * ListView.cpp
 *
 *  Created on: 11 mar 2019
 *      Author: robert
 */
#include <algorithm>

#include "ListView.hpp"
#include <log/log.hpp>

namespace gui
{

    ListView::ListView()
    {

        listItems = new VBox{this, 0, 0, 0, 0};

        scroll = new Rect(this,
                          style::listview::scroll_x,
                          style::listview::scroll_y,
                          style::listview::scroll_w,
                          style::listview::scroll_h);
        scroll->setRadius(style::listview::scroll_radius);
        scroll->setFilled(style::listview::scroll_fill);
        scroll->setFillColor(style::listview::scroll_color);
    }

    ListView::ListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        this->setBorderColor(ColorNoColor);
        direction      = DIRECTION_BOTTOM;
        listContinuous = true;

        listItems = new VBox{this, 0, 0, w, h};
        listItems->setBorderColor(ColorNoColor);

        listItems->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP) {
                direction = DIRECTION_TOP;
                return this->listPageEndReached();
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN) {
                direction = DIRECTION_BOTTOM;
                return this->listPageEndReached();
            }
            else {
                return false;
            }
        };

        scroll = new Rect(this,
                          style::listview::scroll_x,
                          style::listview::scroll_y,
                          style::listview::scroll_w,
                          style::listview::scroll_h);
        scroll->setRadius(style::listview::scroll_radius);
        scroll->setFilled(style::listview::scroll_fill);
        scroll->setFillColor(style::listview::scroll_color);
        scroll->activeItem = false;
    }

    ListView::~ListView()
    {
        clearItems();
    }

    /**
     * For MODE_PAGE:
     * When new number of elements is provided list checks if currently visible items are still valid.
     * If value of the first index is invalid then new value is calculated using number of all elements
     * and defined size of page(number of visible elements).
     * Last index is calculated using value of first index and page size. If value is too big it is reduced
     * to index of the last element.
     * if index of selected element is invalid after change new value is set to the index of the last element
     * in the list.
     */
    void ListView::setElementsCount(int count)
    {
        elementsCount = count;
        refreshList();
    }

    void ListView::drawScroll(bool value)
    {
        drawVerticalScroll = value;
        scroll->visible    = value;
    }

    void ListView::setMaxElements(int value)
    {
        maxElements = value;
    }

    void ListView::setProvider(ListItemProvider *prov)
    {
        this->provider = prov;
    }

    void ListView::clearItems()
    {

        auto temp = listItems->children.size();
        for (unsigned int i = 0; i < temp; i++) {

            auto rm = listItems->children.back();
            listItems->removeWidget(rm);
            delete (rm);
        }
    }

    void ListView::clear()
    {
        clearItems();
        startIndex                  = 0;
        listEndPageReached          = true;
        provider->listDataAvailable = false;
    }

    void ListView::refreshPage()
    {

        if (provider->listDataAvailable && listEndPageReached) {

            auto prevIndex          = startIndex - 1;
            auto pageCalculatedSize = 0;
            auto itemsOnPage        = 0;

            while (pageCalculatedSize <= widgetArea.h) {

                ListItem *item = provider->getItem(0 + itemsOnPage, startIndex, prevIndex, listPageSize, direction);

                if (item != nullptr) {

                    pageCalculatedSize += item->minHeight;
                    item->setSize(item->minWidth, item->minHeight);

                    if (pageCalculatedSize <= widgetArea.h) {

                        listItems->addWidget(item);
                        itemsOnPage++;
                        prevIndex++;
                    }
                    else {
                        listPageSize = itemsOnPage;
                        break;
                    }
                }
                else {
                    break;
                }

                pageCalculatedSize += style::listview::list_span;
                if (pageCalculatedSize <= widgetArea.h) {

                    listSpanItem = new Span(Axis::Y, style::listview::list_span);
                    listItems->addWidget(listSpanItem);
                }
            }

            if (direction == DIRECTION_TOP) {
                setFocusItem(listItems);
                listItems->setFocusOnLastActiveElement();
            }
            if (direction == DIRECTION_BOTTOM) {
                setFocusItem(listItems);
            }

            updateScrollDimenstions();
            listEndPageReached = false;
        }
    }

    void ListView::refreshList()
    {

        if (provider->listDataAvailable) {

            // remove old items
            if (listEndPageReached)
                clearItems();

            if (provider == nullptr)
                return;

            elementsCount = provider->getItemCount();

            refreshPage();
        }
    }

    ListItem *ListView::getSelectedItem()
    {
        return nullptr;
    }

    std::list<DrawCommand *> ListView::buildDrawList()
    {

        // check if widget is visible
        if (visible == false) {
            return std::list<DrawCommand *>();
        }

        refreshList();

        return gui::Rect::buildDrawList();
    }

    void ListView::setPageSize(int size)
    {
        listPageSize = size;
    }

    bool ListView::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        updateScrollDimenstions();
        return true;
    }

    void ListView::updateScrollDimenstions()
    {
        if ((widgetArea.w > 10) && (widgetArea.h > 10)) {
            if (drawVerticalScroll)
                scroll->visible = true;

            uint32_t pagesCount = 1;
            if (listPageSize) {
                pagesCount = (elementsCount % listPageSize == 0) ? elementsCount / listPageSize
                                                                 : elementsCount / listPageSize + 1;
                if (pagesCount == 0) {
                    disableScroll();
                    return;
                }
            }
            uint32_t currentPage = startIndex / listPageSize;
            uint32_t pageHeight  = widgetArea.h / pagesCount;

            scroll->setPosition(widgetArea.w - style::listview::scroll_margin, pageHeight * currentPage);
            scroll->setSize(style::listview::scroll_w, pageHeight);
        }
        // not enough space - disable scroll
        else {
            disableScroll();
        }
    }

    void ListView::disableScroll()
    {
        scroll->setSize(0, 0);
        scroll->visible = false;
    }

    bool ListView::onInput(const InputEvent &inputEvent)

    {
        return listItems->onInput(inputEvent);
    }

    bool ListView::listPageEndReached()
    {

        auto calculateLimit = [&]() {
            return (2 * listPageSize + startIndex <= elementsCount ? 2 * listPageSize : elementsCount - startIndex);
        };

        if (direction == DIRECTION_BOTTOM) {
            if (startIndex + listPageSize > elementsCount) {

                if (listContinuous) {
                    startIndex = 0;
                }
                else {
                    return true;
                }
            }
            else {
                startIndex += listPageSize;
            }

            if (startIndex <= elementsCount - listPageSize)
                provider->requestDBUpdate(startIndex, calculateLimit());
            else
                provider->requestDBUpdate(elementsCount - (elementsCount - startIndex), elementsCount - startIndex);

            provider->listDataAvailable = false;
        }

        if (direction == DIRECTION_TOP) {
            if (startIndex - listPageSize < 0) {

                if (listContinuous) {
                    startIndex = elementsCount - (elementsCount % listPageSize);
                }
                else {
                    return true;
                }
            }
            else {
                startIndex -= listPageSize;
            }

            if (startIndex - listPageSize >= 0)
                provider->requestDBUpdate(startIndex, calculateLimit());
            else
                provider->requestDBUpdate(0, calculateLimit());

            provider->listDataAvailable = false;
        }

        listEndPageReached = true;
        return true;
    }

} /* namespace gui */
