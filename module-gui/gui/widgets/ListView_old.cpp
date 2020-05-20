/*
 * ListView_old.cpp
 *
 *  Created on: 11 mar 2019
 *      Author: robert
 */
#include <algorithm>

#include "ListView_old.hpp"
#include <log/log.hpp>

namespace gui
{

    ListView_old::ListView_old()
        : firstIndex{0}, lastIndex{0}, elementsCount{4}, drawSeparators{true}, drawVerticalScroll{true},
          orientation{ListView_old::ORIENTATION_TOP_DOWN}, maxElements{4},
          selectedIndex{0}, listMode{ListView_old::MODE_PAGE}, pageSize{4}
    {

        setPenFocusWidth(0);
        setPenWidth(0);

        scroll = new Rect(this, 0, 0, 0, 0);
        scroll->setRadius(3);
        scroll->setFilled(true);
        scroll->setFillColor(Color{0, 0});
    }

    ListView_old::ListView_old(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : Rect{parent, x, y, w, h}, firstIndex{0}, lastIndex{0}, elementsCount{4}, drawSeparators{true},
          drawVerticalScroll{true}, orientation{ListView_old::ORIENTATION_TOP_DOWN}, maxElements{4},
          selectedIndex{0}, listMode{ListView_old::MODE_PAGE}, pageSize{4}
    {

        setPenFocusWidth(0);
        setPenWidth(0);

        scroll = new Rect(this, 0, 0, 0, 0);
        scroll->setRadius(3);
        scroll->setFilled(true);
        scroll->setFillColor(Color{0, 0});
    }

    ListView_old::~ListView_old()
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
    void ListView_old::setElementsCount(int count)
    {
        elementsCount = count;
        if (listMode == ListView_old::MODE_PAGE) {
            if (firstIndex > elementsCount)
                firstIndex = (elementsCount / pageSize) * pageSize;
            lastIndex = firstIndex + pageSize - 1;
            if (lastIndex > elementsCount - 1)
                lastIndex = elementsCount - 1;
            if (selectedIndex > lastIndex)
                selectedIndex = lastIndex;
        }
        updateScrollDimenstions();
    }

    void ListView_old::setDrawSeparators(bool value)
    {
        drawSeparators = value;
    }

    void ListView_old::setOrientation(int orientation)
    {
        if (orientation == ListView_old::ORIENTATION_BOTTOM_UP)
            this->orientation = ListView_old::ORIENTATION_BOTTOM_UP;
        else if (orientation == ListView_old::ORIENTATION_TOP_DOWN)
            this->orientation = ListView_old::ORIENTATION_TOP_DOWN;
    }

    void ListView_old::drawScroll(bool value)
    {
        drawVerticalScroll = value;
        scroll->visible    = value;
    }

    void ListView_old::setMaxElements(int value)
    {
        maxElements = value;
    }

    void ListView_old::setProvider(ListItemProvider_old *provider)
    {
        this->provider = provider;
    }

    void ListView_old::clearItems()
    {

        for (auto item : items) {
            removeWidget(item);
            delete (item);
        }

        items.clear();
    }

    void ListView_old::clear()
    {
        clearItems();

        firstIndex    = 0;
        lastIndex     = 0;
        selectedIndex = 0;

        updateScrollDimenstions();
    }

    void ListView_old::updatePageItems()
    {

        int prevIndex = firstIndex - 1;
        for (int i = 0; i < pageSize; i++) {

            ListItem *item = provider->getItem(firstIndex + i, firstIndex, prevIndex, pageSize);
            if (item != nullptr) {
                addWidget(item);
                items.push_back(item);
                prevIndex++;
            }
        }

        // calculate height of the item using list's height and pageSize
        uint32_t itemWidth = widgetArea.w;
        if (drawVerticalScroll)
            itemWidth -= 10;

        int availableHeight = widgetArea.h - pageSize;
        if (availableHeight < 0)
            availableHeight = 0;
        int itemHeight = availableHeight / pageSize;

        int verticalPosition;
        if (orientation == ORIENTATION_TOP_DOWN)
            verticalPosition = 0;
        else
            verticalPosition = widgetArea.h;

        auto it = items.begin();
        for (unsigned int i = 0; i < items.size(); i++) {
            if (availableHeight > 0) {

                itemHeight            = (*it)->area(Area::Min).h;
                gui::BoundingBox bbox = (*it)->widgetArea;
                bbox.h                = itemHeight;
                bbox.w                = itemWidth;

                if (orientation == ORIENTATION_TOP_DOWN) {
                    bbox.y = verticalPosition;
                    verticalPosition += itemHeight + 1; // 1 for separator
                }
                else {
                    verticalPosition -= itemHeight - 1; // 1 for separator
                    bbox.y = verticalPosition;
                }

                (*it)->setBoundingBox(bbox);

                // if list has focus and it is visible mark selected element
                if (visible) {
                    if ((int)(i + firstIndex) == selectedIndex)
                        (*it)->setFocus(true);
                }

                availableHeight -= itemHeight;
            }
            std::advance(it, 1);
        }
    }

    bool ListView_old::onInput(const InputEvent &inputEvent)
    {

        if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_ENTER)) {
            return onActivated(nullptr);
        }

        if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_UP)) {
            if (orientation == ORIENTATION_TOP_DOWN) {
                if (listMode == MODE_PAGE) {
                    if (selectedIndex > firstIndex) {
                        selectedIndex--;
                        return true;
                    }
                    if (selectedIndex == firstIndex) {
                        if (firstIndex > 0) {
                            firstIndex -= pageSize;
                            if (firstIndex < 0)
                                firstIndex = 0;
                            lastIndex = firstIndex + pageSize - 1;
                            if (lastIndex > elementsCount - 1)
                                lastIndex = elementsCount - 1;
                            selectedIndex = lastIndex;
                            updateScrollDimenstions();
                            return true;
                        }
                    }
                }
                else {
                    // TODO implement continuous mode
                }
            }
            else if (orientation == ORIENTATION_BOTTOM_UP) {
                // TODO implement BOTTOM_UP orientation
            }
        }
        else if ((inputEvent.state == InputEvent::State::keyReleasedShort) &&
                 (inputEvent.keyCode == KeyCode::KEY_DOWN)) {
            if (orientation == ORIENTATION_TOP_DOWN) {
                if (listMode == MODE_PAGE) {
                    if (selectedIndex < lastIndex) {
                        selectedIndex++;
                        return true;
                    }
                    if (selectedIndex == lastIndex) {
                        if (lastIndex < elementsCount - 1) {
                            firstIndex += pageSize;
                            if (firstIndex > elementsCount - 1)
                                firstIndex = elementsCount - 1;
                            lastIndex = firstIndex + pageSize - 1;
                            if (lastIndex > elementsCount - 1)
                                lastIndex = elementsCount - 1;
                            selectedIndex = firstIndex;
                            updateScrollDimenstions();
                            return true;
                        }
                    }
                }
                else { // continuous mode
                }
            }
            else { // BOTTOM_UP
            }
        }
        return false;
    }

    void ListView_old::updateContinousItems()
    {}

    // TODO reuse items if possible
    void ListView_old::updateItems()
    {

        // remove old items
        clearItems();

        if (provider == nullptr)
            return;

        elementsCount = provider->getItemCount();

        if (listMode == ListView_old::MODE_PAGE) {
            updatePageItems();
        }
        else if (listMode == ListView_old::MODE_CONTINUOUS) {
            updateContinousItems();
        }
    }

    ListItem *ListView_old::getSelectedItem()
    {
        // return object only if there are any items in the list
        if (elementsCount) {
            auto it = items.begin();
            std::advance(it, selectedIndex - firstIndex);
            return *it;
        }
        return nullptr;
    }

    std::list<DrawCommand *> ListView_old::buildDrawList()
    {

        // check if widget is visible
        if (visible == false) {
            return std::list<DrawCommand *>();
        }

        updateItems();

        // uint8_t* bBuf = gui::Rect::buildDrawList(bSize, bCount);
        return gui::Rect::buildDrawList();
    }

    void ListView_old::setMode(int mode)
    {
        if ((mode == ListView_old::MODE_PAGE) || (mode == ListView_old::MODE_CONTINUOUS))
            listMode = mode;
    }

    void ListView_old::setPageSize(int size)
    {
        pageSize = size;
    }

    bool ListView_old::onActivated(void *data)
    {

        // return if list doesn't have focus
        if (focus == false)
            return false;

        // if there are any elements in the list
        if (elementsCount > 0) {
            // select item that has focus
            auto it = items.begin();
            std::advance(it, selectedIndex - firstIndex);
            return (*it)->onActivated(data);
        }

        return false;
    }

    bool ListView_old::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        updateScrollDimenstions();
        return true;
    }

    void ListView_old::updateScrollDimenstions()
    {
        if ((widgetArea.w > 10) && (widgetArea.h > 10)) {
            if (drawVerticalScroll)
                scroll->visible = true;

            uint32_t pagesCount = 1;
            if (pageSize) {
                pagesCount = (elementsCount % pageSize == 0) ? elementsCount / pageSize : elementsCount / pageSize + 1;
                if (pagesCount == 0) {
                    disableScroll();
                    return;
                }
            }

            uint32_t currentPage = selectedIndex / pageSize;
            uint32_t pageHeight  = widgetArea.h / pagesCount;

            scroll->setPosition(widgetArea.w - 7, pageHeight * currentPage);
            scroll->setSize(7, pageHeight);
        }
        // not enough space - disable scroll
        else {
            disableScroll();
        }
    }

    void ListView_old::disableScroll()
    {
        scroll->setSize(0, 0);
        scroll->visible = false;
    }

} /* namespace gui */
