// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Image.hpp"
#include "InputEvent.hpp"
#include "SideListView.hpp"
#include "Style.hpp"

namespace gui
{
    SideListView::SideListView(Item *parent,
                               unsigned int x,
                               unsigned int y,
                               unsigned int w,
                               unsigned int h,
                               std::shared_ptr<ListItemProvider> prov,
                               PageBarType pageBarType)
        : Rect{parent, x, y, w, h}, ListViewEngine(prov)
    {
        setEdges(RectangleEdge::All);

        bodyOverlay = new VBox{this, 0, 0, w, h};
        bodyOverlay->setEdges(RectangleEdge::None);

        if (pageBarType != PageBarType::None) {
            createPageBar();
            applyScrollCallbacks();
        }

        body = new HBox{bodyOverlay, 0, 0, 0, 0};
        body->setMaximumSize(w, h);
        body->setAlignment(Alignment::Horizontal::Center);
        body->setEdges(RectangleEdge::None);
        bodyOverlay->resizeItems();

        body->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_RF) && pageLoaded) {
                return this->requestPreviousPage();
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER) && pageLoaded) {
                return this->requestNextPage();
            }
            else {
                return false;
            }
        };

        focusChangedCallback = [this]([[maybe_unused]] Item &item) -> bool {
            if (focus) {
                setFocus();
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        type = gui::ItemType::LIST;
    }

    auto SideListView::createPageBar() -> void
    {
        pageBar = new HBarGraph(bodyOverlay, 0U, 0U, 0);
        pageBar->setMinimumHeight(style::sidelistview::progress_bar::h);
        pageBar->setMargins(Margins(style::sidelistview::progress_bar::margin_left,
                                    style::sidelistview::progress_bar::margin_top,
                                    style::sidelistview::progress_bar::margin_right,
                                    style::sidelistview::progress_bar::margin_bottom));
        pageBar->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        pageBar->activeItem = false;
    }

    auto SideListView::applyScrollCallbacks() -> void
    {
        updateScrollCallback = [this](ListViewScrollUpdateData data) {
            if (pageBar != nullptr) {
                auto elementsOnPage = body->widgetArea.w / data.elementMinimalSpaceRequired;
                auto pagesCount     = data.elementsCount % elementsOnPage == 0 ? data.elementsCount / elementsOnPage
                                                                               : data.elementsCount / elementsOnPage + 1;
                auto currentPage    = data.startIndex / elementsOnPage + 1;

                pageBar->setMaximum(pagesCount);
                pageBar->createGraph();
                pageBar->setValue(currentPage);
            }
        };
    }

    auto SideListView::setFocus() -> void
    {
        if (!focus) {
            return;
        }
        setFocusItem(body);

        ListViewEngine::setFocus();
    }

    auto SideListView::onInput(const InputEvent &inputEvent) -> bool
    {
        if (body->onInput(inputEvent)) {
            return true;
        }
        if (body->borderCallback && body->borderCallback(inputEvent)) {
            return true;
        }
        return false;
    }
} /* namespace gui */
