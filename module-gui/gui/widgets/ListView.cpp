// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListView.hpp"
#include "InputEvent.hpp"
#include "cassert"
#include <log/log.hpp>

namespace gui
{
    ListViewScroll::ListViewScroll(
        Item *parent, unsigned int x, unsigned int y, unsigned int w, unsigned int h, listview::ScrollBarType type)
        : Rect{parent, x, y, w, h}, type(type)
    {

        setRadius(style::listview::scroll::radius);
        setFilled(true);
        setFillColor(style::listview::scroll::color);
        activeItem = false;
    }

    void ListViewScroll::updateProportional(const ListViewScrollUpdateData &data)
    {
        double scrollStep =
            static_cast<double>((parent->widgetArea.h - topMargin)) / static_cast<double>(data.elementsCount);

        auto scrollH = scrollStep * data.listPageSize;
        auto scrollY = scrollStep * data.startIndex > 0 ? scrollStep * data.startIndex : topMargin;

        setArea(BoundingBox(
            parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
    }

    void ListViewScroll::updateFixed(const ListViewScrollUpdateData &data)
    {
        auto elementsOnPage = parent->widgetArea.h / data.elementMinimalSpaceRequired;

        pagesCount = data.elementsCount % elementsOnPage == 0 ? data.elementsCount / elementsOnPage
                                                              : data.elementsCount / elementsOnPage + 1;

        currentPage = data.startIndex / elementsOnPage;

        auto scrollH = (parent->widgetArea.h - topMargin) / pagesCount;
        auto scrollY = scrollH * currentPage > 0 ? scrollH * currentPage : topMargin;

        setArea(BoundingBox(
            parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
    }

    void ListViewScroll::updatePreRendered(const ListViewScrollUpdateData &data)
    {
        if (data.startIndex != storedStartIndex) {
            if (data.direction == listview::Direction::Bottom) {
                if (data.boundaries == Boundaries::Continuous && (data.startIndex == 0)) {
                    currentPage = 0;
                }
                else if (currentPage + 1 < pagesCount) {
                    currentPage++;
                }
            }
            else {
                if (data.boundaries == Boundaries::Continuous && storedStartIndex == 0) {
                    currentPage = pagesCount - 1;
                }
                else if (currentPage > 0 && storedStartIndex != 0) {
                    currentPage--;
                }
            }
        }

        storedStartIndex = data.startIndex;

        auto scrollH = parent->widgetArea.h / pagesCount;
        auto scrollY = currentPage * scrollH > 0 ? currentPage * scrollH : topMargin;

        setArea(BoundingBox(
            parent->widgetArea.w - style::listview::scroll::margin, scrollY, style::listview::scroll::w, scrollH));
    }

    void ListViewScroll::updateStartConditions(const ListViewScrollSetupData &data)
    {
        storedStartIndex = data.storedStartIndex;
        currentPage      = data.currentPage;
        pagesCount       = data.pagesCount;
    }

    bool ListViewScroll::shouldShowScroll(unsigned int currentPageSize, unsigned int elementsCount)
    {
        return ((parent->widgetArea.w > style::listview::scroll::min_space) &&
                (parent->widgetArea.h > style::listview::scroll::min_space) && currentPageSize < elementsCount);
    }

    void ListViewScroll::update(const ListViewScrollUpdateData &data)
    {
        if (shouldShowScroll(data.listPageSize, data.elementsCount)) {

            switch (type) {
            case listview::ScrollBarType::Proportional:
                updateProportional(data);
                break;
            case listview::ScrollBarType::Fixed:
                updateFixed(data);
                break;
            case listview::ScrollBarType::PreRendered:
                updatePreRendered(data);
                break;
            case listview::ScrollBarType::None:
                break;
            }

            setVisible(true);
        }
        else
            setVisible(false);
    }
    void ListViewScroll::setTopMargin(int _topMargin)
    {
        topMargin = _topMargin;
    }

    ListView::ListView(Item *parent,
                       unsigned int x,
                       unsigned int y,
                       unsigned int w,
                       unsigned int h,
                       std::shared_ptr<ListItemProvider> prov,
                       listview::ScrollBarType scrollBarType)
        : Rect{parent, x, y, w, h}, ListViewEngine(prov)
    {
        this->setEdges(RectangleEdge::None);

        body = new VBox{this, 0, 0, w, h};
        body->setAlignment(Alignment::Vertical::Top);
        body->setEdges(RectangleEdge::None);

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_UP) && pageLoaded) {
                return this->requestPreviousPage();
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN) && pageLoaded) {
                return this->requestNextPage();
            }
            else {
                return false;
            }
        };

        inputCallback = [&](Item &item, const InputEvent &event) { return body->onInput(event); };

        focusChangedCallback = [this]([[maybe_unused]] Item &item) -> bool {
            if (focus) {
                setFocus();
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        body->parentOnRequestedResizeCallback = [this]() {
            if (pageLoaded)
                recalculateOnBoxRequestedResize();
        };

        checkFullRenderRequirementCallback = [this]() {
            if (scroll && scroll->type == listview::ScrollBarType::PreRendered) {
                requestFullListRender = true;
            }
        };

        if (scrollBarType != listview::ScrollBarType::None) {
            scroll = new ListViewScroll(this,
                                        style::listview::scroll::x,
                                        style::listview::scroll::y,
                                        style::listview::scroll::w,
                                        style::listview::scroll::h,
                                        scrollBarType);
        }

        applyScrollCallbacks();

        type = gui::ItemType::LIST;
    }

    void ListView::applyScrollCallbacks()
    {
        setupScrollCallback = [this](ListViewScrollSetupData data) {
            if (scroll) {
                scroll->updateStartConditions(data);
            }
        };

        updateScrollCallback = [this](ListViewScrollUpdateData data) {
            if (scroll) {
                scroll->update(data);
            }
        };

        resizeScrollCallback = [this]() {
            if (scroll && scroll->shouldShowScroll(currentPageSize, elementsCount)) {
                body->setSize(style::listview::item_width_with_scroll, body->getHeight());
            }
            else {
                body->setSize(style::window::default_body_width, body->getHeight());
            }
        };
    }

    void ListView::clear()
    {
        if (scroll) {
            scroll->setVisible(false);
        }
        ListViewEngine::clear();
    }

    void ListView::setFocus()
    {
        if (!focus) {
            return;
        }
        setFocusItem(body);

        ListViewEngine::setFocus();
    }

    void ListView::setScrollTopMargin(int value)
    {
        if (scroll) {
            scroll->setTopMargin(value);
        }
    }

    void ListView::setAlignment(const Alignment &value)
    {
        if (body->getAlignment() != value) {
            body->setAlignment(value);
        }
    }

    bool ListView::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        body->setSize(body->getWidth(), newDim.h);

        if (updateScrollCallback) {
            updateScrollCallback(ListViewScrollUpdateData{startIndex,
                                                          currentPageSize,
                                                          elementsCount,
                                                          provider->getMinimalItemSpaceRequired(),
                                                          direction,
                                                          boundaries});
        }

        return true;
    }

    auto ListView::handleRequestResize([[maybe_unused]] const Item *child, Length request_w, Length request_h) -> Size
    {
        return Size(request_w, request_h);
    }

} /* namespace gui */
