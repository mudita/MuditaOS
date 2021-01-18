// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListView.hpp"
#include "InputEvent.hpp"
#include "cassert"
#include <log/log.hpp>

namespace gui
{

    ListViewScroll::ListViewScroll(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : Rect{parent, x, y, w, h}
    {

        setRadius(style::listview::scroll::radius);
        setFilled(true);
        setFillColor(style::listview::scroll::color);
        activeItem = false;
    }

    bool ListViewScroll::shouldShowScroll(unsigned int currentPageSize, unsigned int elementsCount)
    {

        return ((parent->widgetArea.w > style::listview::scroll::min_space) &&
                (parent->widgetArea.h > style::listview::scroll::min_space) && currentPageSize < elementsCount);
    }

    void ListViewScroll::update(unsigned int startIndex,
                                unsigned int currentPageSize,
                                unsigned int elementsCount,
                                int topMargin)
    {
        if (shouldShowScroll(currentPageSize, elementsCount)) {

            assert(elementsCount != 0);
            double scrollStep =
                static_cast<double>((parent->widgetArea.h - topMargin)) / static_cast<double>(elementsCount);

            auto scrollH = scrollStep * currentPageSize;
            auto scrollY = scrollStep * startIndex > 0 ? scrollStep * startIndex : topMargin;

            setArea(BoundingBox(
                parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
            setVisible(true);
        }
        else
            setVisible(false);
    }

    ListView::ListView()
    {
        body = new VBox{this, 0, 0, 0, 0};

        scroll = new ListViewScroll(this,
                                    style::listview::scroll::x,
                                    style::listview::scroll::y,
                                    style::listview::scroll::w,
                                    style::listview::scroll::h);
        type   = gui::ItemType::LIST;
    }

    ListView::ListView(Item *parent,
                       uint32_t x,
                       uint32_t y,
                       uint32_t w,
                       uint32_t h,
                       std::shared_ptr<ListItemProvider> prov,
                       style::listview::ScrollBarType scrollBarType)
        : Rect{parent, x, y, w, h}
    {

        this->setBorderColor(ColorNoColor);

        body = new VBox{this, 0, 0, w, h};
        body->setAlignment(Alignment::Vertical::Top);
        body->setEdges(RectangleEdge::None);

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_UP && pageLoaded) {
                return this->requestPreviousPage();
            }
            else if (inputEvent.keyCode == KeyCode::KEY_DOWN && pageLoaded) {
                return this->requestNextPage();
            }
            else {
                return false;
            }
        };

        body->parentOnRequestedResizeCallback = [this]() {
            if (pageLoaded)
                recalculateOnBoxRequestedResize();
        };

        if (scrollBarType != style::listview::ScrollBarType::None) {
            scroll = new ListViewScroll(this,
                                        style::listview::scroll::x,
                                        style::listview::scroll::y,
                                        style::listview::scroll::w,
                                        style::listview::scroll::h);
        }

        setProvider(std::move(prov));

        type = gui::ItemType::LIST;
    }

    ListView::~ListView()
    {
        clearItems();
    }

    void ListView::setElementsCount(unsigned int count)
    {
        elementsCount = count;
    }

    void ListView::setBoundaries(style::listview::Boundaries value)
    {
        boundaries = value;
    }

    void ListView::setScrollTopMargin(int value)
    {
        scrollTopMargin = value;
    }

    void ListView::setOrientation(style::listview::Orientation value)
    {
        orientation = value;

        if (orientation == style::listview::Orientation::TopBottom) {
            body->setAlignment(Alignment::Vertical::Top);
        }
        else {
            body->setAlignment(Alignment::Vertical::Bottom);
        }
    }

    void ListView::setFocusOnElement(unsigned int elementNumber)
    {
        body->setFocusOnElement(elementNumber);
    }

    void ListView::setProvider(std::shared_ptr<ListItemProvider> prov)
    {
        if (prov != nullptr) {
            provider       = prov;
            provider->list = this;
        }
    }

    bool ListView::isEmpty() const noexcept
    {
        return elementsCount == 0;
    }

    void ListView::rebuildList(style::listview::RebuildType rebuildType, unsigned int dataOffset, bool forceRebuild)
    {
        if (pageLoaded || forceRebuild) {

            setElementsCount(provider->requestRecordsCount());

            setup(rebuildType, dataOffset);

            // If deletion operation caused last page to be removed request previous one.
            if ((startIndex != 0 && startIndex == elementsCount)) {
                requestPreviousPage();
            }
            else {
                provider->requestRecords(startIndex, calculateLimit());
            }
        }
        else {
            rebuildRequests.push_front({rebuildType, dataOffset});
        }
    }

    void ListView::reSendLastRebuildRequest()
    {
        rebuildList(lastRebuildRequest.first, lastRebuildRequest.second, true);
    }

    void ListView::setup(style::listview::RebuildType rebuildType, unsigned int dataOffset)
    {
        if (rebuildType == style::listview::RebuildType::Full) {
            setStartIndex();
            storedFocusIndex = 0;
        }
        else if (rebuildType == style::listview::RebuildType::OnOffset) {
            if (dataOffset < elementsCount) {
                startIndex       = dataOffset;
                storedFocusIndex = 0;
            }
            else {
                LOG_ERROR("Requested rebuild on index greater than elements count");
            }
        }
        else if (rebuildType == style::listview::RebuildType::InPlace) {

            storedFocusIndex = body->getFocusItemIndex();

            if (direction == style::listview::Direction::Top) {
                int position     = currentPageSize - 1 - storedFocusIndex;
                storedFocusIndex = std::abs(position);
            }
        }

        lastRebuildRequest = {rebuildType, dataOffset};

        body->setReverseOrder(false);
        direction = style::listview::Direction::Bottom;
    }

    std::shared_ptr<ListItemProvider> ListView::getProvider()
    {
        return provider;
    }

    void ListView::setAlignment(const Alignment &value)
    {
        if (body->getAlignment() != value) {
            body->setAlignment(value);
        }
    }

    void ListView::clear()
    {
        clearItems();
        setStartIndex();
        body->setReverseOrder(false);
        direction = style::listview::Direction::Bottom;
    }

    void ListView::clearItems()
    {
        body->setFocusItem(nullptr);

        while (auto el = body->children.back()) {

            if (el->type == ItemType::LIST_ITEM) {

                if (!dynamic_cast<ListItem *>(el)->deleteByList) {
                    body->removeWidget(el);
                }
                else {
                    body->erase(el);
                }
            }
            else
                body->erase(el);
        }
    }

    void ListView::refresh()
    {
        if (provider == nullptr) {
            LOG_ERROR("ListView Data provider not exist");
            return;
        }

        onElementsCountChanged();

        clearItems();

        addItemsOnPage();

        setFocus();
        if (scroll) {
            scroll->update(startIndex, currentPageSize, elementsCount, scrollTopMargin);
        }
        resizeWithScroll();
        pageLoaded = true;

        // Check if there were queued rebuild Requests - if so rebuild list again.
        if (!rebuildRequests.empty()) {
            rebuildList(rebuildRequests.back().first, rebuildRequests.back().second);
            rebuildRequests.pop_back();
        }

        fillFirstPage();
    }

    void ListView::onProviderDataUpdate()
    {
        refresh();
    }

    Order ListView::getOrderFromDirection()
    {
        if (direction == style::listview::Direction::Bottom)
            return Order::Next;

        return Order::Previous;
    }

    void ListView::setStartIndex()
    {
        if (orientation == style::listview::Orientation::TopBottom) {
            startIndex = 0;
        }
        else {
            startIndex = elementsCount;
        }
    }

    void ListView::recalculateStartIndex()
    {
        if (direction == style::listview::Direction::Top) {

            startIndex = startIndex < currentPageSize ? 0 : startIndex - currentPageSize;
        }
    }

    void ListView::fillFirstPage()
    {
        // Check if first page is filled with items. If not reload page to be filed with items. Check for both
        // Orientations.
        if (orientation == style::listview::Orientation::TopBottom && direction == style::listview::Direction::Top &&
            startIndex == 0) {
            if (body->getSizeLeft() > provider->getMinimalItemHeight()) {
                focusOnLastItem = true;
                rebuildList();
            }
        }

        if (orientation == style::listview::Orientation::BottomTop && direction == style::listview::Direction::Bottom &&
            startIndex + currentPageSize == elementsCount) {
            if (body->getSizeLeft() > provider->getMinimalItemHeight()) {
                focusOnLastItem = true;
                rebuildList();
            }
        }
    }

    void ListView::resizeWithScroll()
    {
        if (scroll && scroll->shouldShowScroll(currentPageSize, elementsCount)) {
            body->setSize(style::listview::item_width_with_scroll, body->getHeight());
        }
        else {
            body->setSize(style::window::default_body_width, body->getHeight());
        }
    }

    void ListView::addItemsOnPage()
    {
        currentPageSize = 0;

        ListItem *item = nullptr;

        while ((item = provider->getItem(getOrderFromDirection())) != nullptr) {

            body->addWidget(item);

            if (item->visible != true) {
                break;
            }

            currentPageSize++;
        }

        recalculateStartIndex();
    }

    void ListView::setFocus()
    {
        setFocusItem(body);

        if (storedFocusIndex != 0) {
            if (!body->setFocusOnElement(storedFocusIndex)) {
                body->setFocusOnLastElement();
            }
        }

        if (focusOnLastItem) {
            body->setFocusOnLastElement();
            focusOnLastItem = false;
        }
    };

    void ListView::onElementsCountChanged()
    {
        if (isEmpty()) {
            if (emptyListCallback) {
                emptyListCallback();
            }
        }
        else if (notEmptyListCallback) {
            notEmptyListCallback();
        }
    }

    bool ListView::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        body->setSize(body->getWidth(), newDim.h);
        if (scroll) {
            scroll->update(startIndex, currentPageSize, elementsCount, scrollTopMargin);
        }

        return true;
    }

    auto ListView::handleRequestResize([[maybe_unused]] const Item *child,
                                       unsigned short request_w,
                                       unsigned short request_h) -> Size
    {

        return Size(request_w, request_h);
    }

    void ListView::recalculateOnBoxRequestedResize()
    {
        if (currentPageSize != body->getVisibleChildrenCount()) {

            unsigned int diff = currentPageSize < body->getVisibleChildrenCount()
                                    ? 0
                                    : currentPageSize - body->getVisibleChildrenCount();
            currentPageSize = body->getVisibleChildrenCount();

            if (direction == style::listview::Direction::Top) {
                startIndex += diff;
            }
            else {
                startIndex = startIndex < diff ? 0 : startIndex - diff;
            }

            fillFirstPage();
        }
    }

    bool ListView::onInput(const InputEvent &inputEvent)
    {
        return body->onInput(inputEvent);
    }

    unsigned int ListView::calculateMaxItemsOnPage()
    {
        assert(provider->getMinimalItemHeight() != 0);
        auto count = widgetArea.h / provider->getMinimalItemHeight();

        return count;
    }

    unsigned int ListView::calculateLimit(style::listview::Direction value)
    {
        auto minLimit =
            (2 * currentPageSize > calculateMaxItemsOnPage() ? 2 * currentPageSize : calculateMaxItemsOnPage());
        if (value == style::listview::Direction::Bottom)
            return (minLimit + startIndex <= elementsCount ? minLimit : elementsCount - startIndex);
        else
            return minLimit < startIndex ? minLimit : startIndex;
    }

    bool ListView::requestNextPage()
    {
        if (startIndex + currentPageSize >= elementsCount && boundaries == style::listview::Boundaries::Continuous) {

            startIndex = 0;
        }
        else if (startIndex + currentPageSize >= elementsCount && boundaries == style::listview::Boundaries::Fixed) {

            return false;
        }
        else {

            startIndex = startIndex <= elementsCount - currentPageSize ? startIndex + currentPageSize
                                                                       : elementsCount - (elementsCount - startIndex);
        }

        direction = style::listview::Direction::Bottom;
        body->setReverseOrder(false);
        pageLoaded       = false;
        storedFocusIndex = 0;
        provider->requestRecords(startIndex, calculateLimit());

        return true;
    }

    bool ListView::requestPreviousPage()
    {
        auto topFetchIndex = 0;
        auto limit         = 0;

        if (startIndex == 0 && boundaries == style::listview::Boundaries::Continuous) {

            topFetchIndex = elementsCount - (elementsCount % currentPageSize);
            startIndex    = elementsCount;
            limit         = calculateLimit(style::listview::Direction::Top) - topFetchIndex;
        }
        else if (startIndex == 0 && boundaries == style::listview::Boundaries::Fixed) {

            return false;
        }
        else {

            limit         = calculateLimit(style::listview::Direction::Top);
            topFetchIndex = startIndex < calculateLimit(style::listview::Direction::Top)
                                ? 0
                                : startIndex - calculateLimit(style::listview::Direction::Top);
        }

        direction = style::listview::Direction::Top;
        body->setReverseOrder(true);
        pageLoaded       = false;
        storedFocusIndex = 0;
        provider->requestRecords(topFetchIndex, limit);

        return true;
    }
} /* namespace gui */
