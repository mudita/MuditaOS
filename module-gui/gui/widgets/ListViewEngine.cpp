// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListViewEngine.hpp"

#include <cassert>
#include <log/log.hpp>

namespace gui
{
    ListViewEngine::ListViewEngine(std::shared_ptr<ListItemProvider> prov)
    {
        setProvider(std::move(prov));
    }

    ListViewEngine::~ListViewEngine()
    {
        clear();
    }

    void ListViewEngine::setElementsCount(unsigned count)
    {
        if ((elementsCount != count) || (elementsCount == listview::nPos)) {
            onElementsCountChanged(count);
        }
    }

    void ListViewEngine::setBoundaries(Boundaries value)
    {
        boundaries = value;
    }

    void ListViewEngine::setOrientation(listview::Orientation value)
    {
        orientation = value;

        if (orientation == listview::Orientation::TopBottom) {
            body->setAlignment(Alignment::Vertical::Top);
        }
        else {
            body->setAlignment(Alignment::Vertical::Bottom);
        }
    }

    void ListViewEngine::setProvider(std::shared_ptr<ListItemProvider> prov)
    {
        if (prov != nullptr) {
            provider       = prov;
            provider->list = this;
        }
    }

    bool ListViewEngine::isEmpty() const noexcept
    {
        return elementsCount == 0;
    }

    void ListViewEngine::rebuildList(listview::RebuildType rebuildType, unsigned dataOffset, bool forceRebuild)
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

    void ListViewEngine::reSendLastRebuildRequest()
    {
        rebuildList(lastRebuildRequest.first, lastRebuildRequest.second, true);
    }

    void ListViewEngine::prepareFullRebuild()
    {
        setStartIndex();
        storedFocusIndex = listview::nPos;
    }

    void ListViewEngine::prepareOnOffsetRebuild(unsigned dataOffset)
    {
        if (dataOffset < elementsCount) {
            startIndex       = dataOffset;
            storedFocusIndex = listview::nPos;
        }
        else {
            LOG_ERROR("Requested rebuild on index greater than elements count");
        }
    }

    void ListViewEngine::prepareInPlaceRebuild()
    {
        if (!body->empty()) {
            storedFocusIndex = getFocusItemIndex();
        }
    }

    void ListViewEngine::prepareOnPageElementRebuild(unsigned dataOffset)
    {
        const auto maxItemsOnPage = calculateMaxItemsOnPage();
        startIndex                = (dataOffset / maxItemsOnPage) * maxItemsOnPage;
        storedFocusIndex          = dataOffset % maxItemsOnPage;
    }

    void ListViewEngine::setup(listview::RebuildType rebuildType, unsigned dataOffset)
    {
        switch (rebuildType) {
        case listview::RebuildType::Full:
            prepareFullRebuild();
            break;
        case listview::RebuildType::OnOffset:
            prepareOnOffsetRebuild(dataOffset);
            break;
        case listview::RebuildType::InPlace:
            prepareInPlaceRebuild();
            break;
        case listview::RebuildType::OnPageElement:
            prepareOnPageElementRebuild(dataOffset);
            break;
        }

        if (prepareRebuildCallback) {
            prepareRebuildCallback();
            setElementsCount(provider->requestRecordsCount());
        }

        lastRebuildRequest = {rebuildType, dataOffset};

        body->setReverseOrder(false);
        direction = listview::Direction::Bottom;
    }

    void ListViewEngine::onClose()
    {
        if (!body->empty()) {
            storedFocusIndex = getFocusItemIndex();
        }
        clear();
    }

    unsigned ListViewEngine::getFocusItemIndex()
    {
        auto index = body->getFocusItemIndex();

        if (direction == listview::Direction::Top) {
            const int position = currentPageSize - 1 - index;
            index              = std::abs(position);
        }

        return index;
    }

    std::shared_ptr<ListItemProvider> ListViewEngine::getProvider()
    {
        return provider;
    }

    void ListViewEngine::reset()
    {
        clear();
        setStartIndex();
        body->setReverseOrder(false);
        direction = listview::Direction::Bottom;
    }

    void ListViewEngine::clear()
    {
        body->setFocusItem(nullptr);

        while (const auto el = body->children.back()) {

            if (el->type == ItemType::LIST_ITEM) {

                if (!dynamic_cast<ListItem *>(el)->deleteByList) {
                    body->removeWidget(el);
                }
                else {
                    body->erase(el);
                }
            }
            else {
                body->erase(el);
            }
        }
    }

    void ListViewEngine::refresh()
    {
        if (provider == nullptr) {
            LOG_ERROR("ListView Data provider not exist");
            return;
        }

        clear();
        checkEmptyListCallbacks();

        addItemsOnPage();

        setFocus();

        if (updateScrollCallback) {
            updateScrollCallback(ListViewScrollUpdateData{startIndex,
                                                          currentPageSize,
                                                          elementsCount,
                                                          provider->getMinimalItemSpaceRequired(),
                                                          direction,
                                                          boundaries});
        }

        if (resizeScrollCallback) {
            resizeScrollCallback();
        }

        pageLoaded = true;

        // Check if there were queued rebuild Requests - if so rebuild list again.
        if (!rebuildRequests.empty()) {
            auto request = rebuildRequests.back();
            rebuildRequests.pop_back();
            rebuildList(request.first, request.second);
        }

        updateCountOfElementsAboveCurrentPage();

        fillFirstPage();
    }

    void ListViewEngine::onProviderDataUpdate()
    {
        if (!renderFullList()) {
            return;
        }

        refresh();
    }

    Order ListViewEngine::getOrderFromDirection() const noexcept
    {
        if (direction == listview::Direction::Bottom) {
            return Order::Next;
        }

        return Order::Previous;
    }

    Order ListViewEngine::getOppositeOrderFromDirection() const noexcept
    {
        if (direction == listview::Direction::Bottom) {
            return Order::Previous;
        }

        return Order::Next;
    }

    void ListViewEngine::setStartIndex()
    {
        if (orientation == listview::Orientation::TopBottom) {
            startIndex = 0;
        }
        else {
            startIndex = elementsCount;
        }
    }

    void ListViewEngine::recalculateStartIndex()
    {
        if (direction == listview::Direction::Top) {
            startIndex = startIndex < currentPageSize ? 0 : startIndex - currentPageSize;
        }
    }

    void ListViewEngine::fillFirstPage()
    {
        // Check if first page is filled with items. If not reload page to be filled with items. Check for both
        // Orientations.
        if (orientation == listview::Orientation::TopBottom && direction == listview::Direction::Top &&
            startIndex == 0) {
            if (body->getPrimarySizeLeft() >= provider->getMinimalItemSpaceRequired()) {
                focusOnLastItem = true;

                if (checkFullRenderRequirementCallback) {
                    checkFullRenderRequirementCallback();
                }
                rebuildList();
            }
        }

        if (orientation == listview::Orientation::BottomTop && direction == listview::Direction::Bottom &&
            startIndex + currentPageSize == elementsCount) {
            if (body->getPrimarySizeLeft() >= provider->getMinimalItemSpaceRequired()) {
                focusOnLastItem = true;

                if (checkFullRenderRequirementCallback) {
                    checkFullRenderRequirementCallback();
                }
                rebuildList();
            }
        }
    }

    void ListViewEngine::addItemsOnPage()
    {
        currentPageSize = 0;

        while (const auto item = provider->getItem(getOrderFromDirection())) {

            body->addWidget(item);

            if (!item->visible) {
                // In case model is tracking internal indexes -> undo last get.
                if (requestFullListRender) {
                    provider->getItem(getOppositeOrderFromDirection());
                }
                break;
            }

            currentPageSize++;
        }

        recalculateStartIndex();
    }

    bool ListViewEngine::renderFullList()
    {
        if (!requestFullListRender) {
            return true;
        }

        if (elementsCount != 0 && !requestCompleteData) {
            requestCompleteData = true;
            provider->requestRecords(0, elementsCount);
            return false;
        }

        if (requestCompleteData) {

            unsigned page       = 0;
            auto pageStartIndex = 0;

            clear();

            while (true) {

                addItemsOnPage();

                if (currentPageSize == 0) {
                    break;
                }

                if (currentPageSize + pageStartIndex == elementsCount) {
                    break;
                }

                page += 1;
                pageStartIndex += currentPageSize;

                clear();
            }

            clear();
            requestCompleteData   = false;
            requestFullListRender = false;

            if (lastRebuildRequest.first == listview::RebuildType::Full) {
                if (orientation == listview::Orientation::TopBottom) {
                    if (setupScrollCallback) {
                        setupScrollCallback(ListViewScrollSetupData{startIndex, 0, (page + 1)});
                    }
                }
                else {
                    if (setupScrollCallback) {
                        setupScrollCallback(ListViewScrollSetupData{startIndex, page, (page + 1)});
                    }
                }
            }

            updateCountOfElementsAboveCurrentPage();

            reSendLastRebuildRequest();
            return false;
        }

        return true;
    } // namespace gui

    void ListViewEngine::setFocus()
    {
        if (storedFocusIndex != listview::nPos) {
            if (!body->setFocusOnElement(storedFocusIndex)) {
                body->setFocusOnLastElement();
            }
        }

        if (focusOnLastItem) {
            body->setFocusOnLastElement();
            focusOnLastItem = false;
        }
    }

    void ListViewEngine::onElementsCountChanged(unsigned count)
    {
        if (elementsCount == 0 || count == 0) {
            shouldCallEmptyListCallbacks = true;
        }

        elementsCount = count;

        if (checkFullRenderRequirementCallback) {
            checkFullRenderRequirementCallback();
        }
    }

    void ListViewEngine::checkEmptyListCallbacks()
    {
        if (shouldCallEmptyListCallbacks) {
            if (isEmpty()) {
                if (emptyListCallback) {
                    emptyListCallback();
                }
            }
            else if (notEmptyListCallback) {
                notEmptyListCallback();
            }
            shouldCallEmptyListCallbacks = false;
        }
    }

    void ListViewEngine::recalculateOnBoxRequestedResize()
    {
        if (currentPageSize != body->getVisibleChildrenCount()) {

            const unsigned diff = currentPageSize < body->getVisibleChildrenCount()
                                      ? 0
                                      : currentPageSize - body->getVisibleChildrenCount();
            currentPageSize     = body->getVisibleChildrenCount();

            if (direction == listview::Direction::Top) {
                startIndex += diff;
            }
            else {
                startIndex = startIndex < diff ? 0 : startIndex - diff;
            }

            if (checkFullRenderRequirementCallback) {
                checkFullRenderRequirementCallback();
            }

            rebuildList();
        }
    }

    unsigned ListViewEngine::calculateMaxItemsOnPage()
    {
        assert(provider->getMinimalItemSpaceRequired() != 0);
        const auto count = body->getPrimarySize() / provider->getMinimalItemSpaceRequired();
        return count;
    }

    unsigned ListViewEngine::calculateLimit(listview::Direction value)
    {
        const auto minLimit = std::max(2 * currentPageSize, calculateMaxItemsOnPage());

        if (value == listview::Direction::Bottom) {
            return (minLimit + startIndex <= elementsCount) ? minLimit : (elementsCount - startIndex);
        }
        return std::min(minLimit, startIndex);
    }

    bool ListViewEngine::requestNextPage()
    {
        if (startIndex + currentPageSize >= elementsCount && boundaries == Boundaries::Continuous) {

            startIndex = 0;
        }
        else if (startIndex + currentPageSize >= elementsCount && boundaries == Boundaries::Fixed) {

            return false;
        }
        else {

            startIndex = startIndex <= elementsCount - currentPageSize ? startIndex + currentPageSize
                                                                       : elementsCount - (elementsCount - startIndex);
        }

        direction = listview::Direction::Bottom;
        body->setReverseOrder(false);
        pageLoaded       = false;
        storedFocusIndex = listview::nPos;
        provider->requestRecords(startIndex, calculateLimit());

        return true;
    }

    bool ListViewEngine::requestPreviousPage()
    {
        auto topFetchIndex = 0;
        auto limit         = 0;

        if (startIndex == 0 && boundaries == Boundaries::Continuous) {

            startIndex = elementsCount;
            if (elementsCount > currentPageSize && fetchType == listview::FetchType::Fixed) {

                auto calculateFixedFill =
                    elementsCount % currentPageSize != 0 ? elementsCount % currentPageSize : currentPageSize;
                topFetchIndex = elementsCount - calculateFixedFill;
            }
            else {
                topFetchIndex = elementsCount - calculateLimit(listview::Direction::Top);
            }

            limit = calculateLimit(listview::Direction::Top);
        }
        else if (startIndex == 0 && boundaries == Boundaries::Fixed) {

            return false;
        }
        else {

            limit         = calculateLimit(listview::Direction::Top);
            topFetchIndex = startIndex < calculateLimit(listview::Direction::Top)
                                ? 0
                                : startIndex - calculateLimit(listview::Direction::Top);
        }

        direction = listview::Direction::Top;
        body->setReverseOrder(true);
        pageLoaded       = false;
        storedFocusIndex = listview::nPos;

        provider->requestRecords(topFetchIndex, limit);

        return true;
    }

    void ListViewEngine::updateCountOfElementsAboveCurrentPage()
    {
        unsigned countOfElementsAboveCurrentPage = startIndex;
        if (onElementsAboveOfCurrentPageChangeCallback) {
            onElementsAboveOfCurrentPageChangeCallback(countOfElementsAboveCurrentPage);
        }
    }
} /* namespace gui */
