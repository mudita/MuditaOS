// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListViewWithArrows.hpp"
#include "TextFixedSize.hpp"
#include <InputEvent.hpp>

namespace gui
{
    ListViewWithArrows::ListViewWithArrows(Item *parent,
                                           unsigned int x,
                                           unsigned int y,
                                           unsigned int w,
                                           unsigned int h,
                                           std::shared_ptr<ListItemProvider> prov)
        : Rect{parent, x, y, w, h}, ListViewEngine(prov)
    {
        this->setEdges(RectangleEdge::None);
        createLayout();

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

        body->dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            if (listOverlay->lastBox->visible && direction == listview::Direction::Top) {
                if (startIndex > 1 && listOverlay->firstBox->visible) {
                    startIndex += 1;
                }

                if (startIndex == 1 && !listOverlay->firstBox->visible) {
                    startIndex = 0;
                }
            }

            currentPageSize = body->getVisibleChildrenCount();
            return true;
        };

        applyScrollCallbacks();

        type = gui::ItemType::LIST;
    }

    void ListViewWithArrows::createLayout()
    {
        widgetBody = new VBox(this, 0, 0, widgetArea.w, widgetArea.h);
        widgetBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        widgetBody->setEdges(RectangleEdge::None);

        listOverlay = new VThreeBox<VBox, VBox, VBox>(widgetBody, 0, 0, 0, 0);
        listOverlay->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        listOverlay->setEdges(RectangleEdge::None);

        listOverlay->firstBox = new VBox(listOverlay);
        listOverlay->firstBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        listOverlay->firstBox->setEdges(RectangleEdge::None);
        listOverlay->firstBox->setVisible(false);

        arrowTop = new ImageBox(listOverlay->firstBox, new Image("bell_arrow_top_W_M"));
        arrowTop->setMinimumSizeToFitImage();

        listOverlay->centerBox = new VBox(listOverlay);
        listOverlay->centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        listOverlay->centerBox->setEdges(RectangleEdge::None);

        body = new VBox(listOverlay->centerBox);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        body->setEdges(RectangleEdge::None);

        listOverlay->lastBox = new VBox(listOverlay);
        listOverlay->lastBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        listOverlay->lastBox->setEdges(RectangleEdge::None);
        listOverlay->lastBox->setVisible(false);

        arrowDown = new ImageBox(listOverlay->lastBox, new Image("bell_arrow_down_W_M"));
        arrowDown->setMinimumSizeToFitImage();
    }

    void ListViewWithArrows::applySizeRestrictions(unsigned int w,
                                                   unsigned int h,
                                                   unsigned int outerLayoutsH,
                                                   int outerLayoutsMargin)
    {
        listOverlay->setMinimumSize(w, h);
        body->setMaximumSize(w, h);

        listOverlay->firstBox->setMinimumSize(w, outerLayoutsH);
        listOverlay->centerBox->setMaximumSize(w, h);
        listOverlay->centerBox->setMargins(Margins(0, outerLayoutsMargin, 0, outerLayoutsMargin));
        listOverlay->lastBox->setMinimumSize(w, outerLayoutsH);

        widgetBody->resizeItems();
    }

    void ListViewWithArrows::setListTitle(const std::string &title)
    {
        titleBody = new TextFixedSize(listOverlay->firstBox);
        titleBody->drawUnderline(false);
        titleBody->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        titleBody->setFont(style::window::font::large);
        titleBody->setMinimumSize(listOverlay->firstBox->widgetMinimumArea.w,
                                  listOverlay->firstBox->widgetMinimumArea.h);
        titleBody->setEdges(RectangleEdge::None);
        titleBody->setEditMode(EditMode::Browse);
        titleBody->setText(title);

        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        listOverlay->firstBox->setVisible(true);

        listOverlay->resizeItems();
        // Second resize is needed as we need to first apply max size for center box and next extra margins.
        listOverlay->resizeItems();

        arrowTop->setVisible(false);
        titleBody->setVisible(true);
        listOverlay->firstBox->resizeItems();
    }

    void ListViewWithArrows::applyScrollCallbacks()
    {
        updateScrollCallback = [this](ListViewScrollUpdateData data) {
            auto elementsOnPage = listOverlay->centerBox->widgetArea.h / data.elementMinimalSpaceRequired;

            if (boundaries == Boundaries::Continuous) {
                if (elementsOnPage == data.elementsCount) {
                    listOverlay->firstBox->setVisible(true);
                    listOverlay->lastBox->setVisible(false);
                }
                else if (elementsOnPage < data.elementsCount) {
                    listOverlay->firstBox->setVisible(true);
                    listOverlay->lastBox->setVisible(true);
                }
                else {
                    listOverlay->firstBox->setVisible(false);
                    listOverlay->lastBox->setVisible(false);
                }
            }
            else if (boundaries == Boundaries::Fixed) {
                if (elementsOnPage + data.startIndex < data.elementsCount) {
                    listOverlay->lastBox->setVisible(true);
                }
                else {
                    listOverlay->lastBox->setVisible(false);
                }

                if (data.startIndex == 0 && titleBody) {
                    titleBody->setVisible(true);
                    arrowTop->setVisible(false);
                    listOverlay->firstBox->setVisible(true);
                }
                else if (data.startIndex > 1) {
                    if (titleBody) {
                        titleBody->setVisible(false);
                    }
                    arrowTop->setVisible(true);
                    listOverlay->firstBox->setVisible(true);
                }
                else {
                    listOverlay->firstBox->setVisible(false);
                }
            }

            listOverlay->resizeItems();
            // Second resize is needed as we need to first apply max size for center box and next extra margins.
            listOverlay->resizeItems();
        };
    }

    void ListViewWithArrows::setFocus()
    {
        if (!focus) {
            return;
        }
        setFocusItem(body);

        ListViewEngine::setFocus();
    }

    void ListViewWithArrows::setAlignment(const Alignment &value)
    {
        if (body->getAlignment() != value) {
            body->setAlignment(value);
        }
    }

    bool ListViewWithArrows::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        body->setSize(body->getWidth(), newDim.h);

        return true;
    }

} /* namespace gui */
