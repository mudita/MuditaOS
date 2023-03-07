// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputEvent.hpp"
#include "ListView.hpp"
#include "ScrollBar.hpp"

namespace gui
{
    ListView::ListView(Item *parent,
                       unsigned int x,
                       unsigned int y,
                       unsigned int w,
                       unsigned int h,
                       std::shared_ptr<ListItemProvider> prov,
                       ScrollBarType scrollBarType)
        : Rect{parent, x, y, w, h}, ListViewEngine(std::move(prov))
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
            return false;
        };

        inputCallback = [&]([[maybe_unused]] Item &item, const InputEvent &event) { return body->onInput(event); };

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
            if (pageLoaded) {
                recalculateOnBoxRequestedResize();
            }
        };

        checkFullRenderRequirementCallback = [this]() {
            if (scroll && scroll->type == ScrollBarType::PreRendered) {
                requestFullListRender = true;
            }
        };

        if (scrollBarType != ScrollBarType::None) {
            scroll = new ListScrollBar(parent,
                                        style::listview::scroll::x,
                                        style::listview::scroll::y,
                                        style::listview::scroll::w,
                                        style::listview::scroll::h,
                                        scrollBarType);
        }
        determineFetchType(scrollBarType);
        applyScrollCallbacks();

        type = gui::ItemType::LIST;
    }

    void ListView::applyScrollCallbacks()
    {
        setupScrollCallback = [this](ListViewScrollSetupData const& data) {
            if (scroll) {
                scroll->updateStartConditions(data.storedStartIndex, data.currentPage, data.pagesCount);
            }
        };

        updateScrollCallback = [this](ListViewScrollBarUpdateData const& data) {
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

    void ListView::determineFetchType(ScrollBarType scrollType)
    {
        switch (scrollType) {
        case ScrollBarType::None:
        case ScrollBarType::Fixed:
            fetchType = listview::FetchType::Fixed;
            break;
        case ScrollBarType::Proportional:
        case ScrollBarType::PreRendered:
            fetchType = listview::FetchType::Filled;
            break;
        }
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
            updateScrollCallback(ListViewScrollBarUpdateData{startIndex,
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
