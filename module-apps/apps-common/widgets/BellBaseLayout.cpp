// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBaseLayout.hpp"
#include "Style.hpp"

namespace gui
{
    BellBaseLayout::BellBaseLayout(Item *parent, Position x, Position y, Length w, Length h, bool withSideArrows)
        : VThreeBox(parent, x, y, w, h)
    {
        setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        firstBox = new VBox(this);
        firstBox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::first_layout_min_h);
        firstBox->setMaximumHeight(style::bell_base_layout::outer_layouts_h);
        firstBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        firstBox->setEdges(RectangleEdge::None);
        firstBox->activeItem = false;

        centerBox = new VBox(this);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        centerBox->setMinimumSize(style::bell_base_layout::center_layout_w, style::bell_base_layout::center_layout_h);

        lastBox = new VBox(this);
        lastBox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        lastBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        lastBox->setEdges(RectangleEdge::None);
        lastBox->activeItem = false;

        resizeItems();

        if (withSideArrows) {
            addSideArrows();
        }
    }

    Item *BellBaseLayout::getCenterBox() const noexcept
    {
        if (arrowsThreeBox != nullptr) {
            return arrowsThreeBox->centerBox;
        }
        return centerBox;
    }

    void BellBaseLayout::resizeCenter()
    {
        centerBox->resizeItems();
        if (arrowsThreeBox != nullptr) {
            arrowsThreeBox->resizeItems();
        }
    }

    void BellBaseLayout::resize()
    {
        resizeItems();
        firstBox->resizeItems();
        resizeCenter();
        lastBox->resizeItems();
    }

    void BellBaseLayout::addSideArrows()
    {
        arrowsThreeBox = new HThreeBox<HBox, HBox, HBox>(centerBox);
        arrowsThreeBox->setMinimumSize(style::bell_base_layout::arrows_layout_w,
                                       style::bell_base_layout::center_layout_h);
        arrowsThreeBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        arrowsThreeBox->setEdges(RectangleEdge::None);

        arrowsThreeBox->firstBox = new HBox(arrowsThreeBox);
        arrowsThreeBox->firstBox->setAlignment(Alignment(Alignment::Vertical::Center));
        arrowsThreeBox->firstBox->setEdges(RectangleEdge::None);
        arrowsThreeBox->firstBox->activeItem = false;

        leftArrow = new ImageBox(arrowsThreeBox->firstBox, new Image("bell_arrow_left_W_M"));
        leftArrow->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        leftArrow->setMinimumSizeToFitImage();
        leftArrow->setVisible(true);
        leftArrow->setEdges(RectangleEdge::None);
        arrowsThreeBox->firstBox->setMinimumSize(leftArrow->widgetMinimumArea.w, leftArrow->widgetMinimumArea.h);

        arrowsThreeBox->centerBox = new HBox(arrowsThreeBox);
        arrowsThreeBox->centerBox->setEdges(RectangleEdge::None);
        arrowsThreeBox->centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        arrowsThreeBox->centerBox->setMaximumSize(style::bell_base_layout::center_layout_w,
                                                  style::bell_base_layout::center_layout_h);

        arrowsThreeBox->lastBox = new HBox(arrowsThreeBox);
        arrowsThreeBox->lastBox->setAlignment(Alignment(Alignment::Vertical::Center));
        arrowsThreeBox->lastBox->setEdges(RectangleEdge::None);
        arrowsThreeBox->lastBox->activeItem = false;

        rightArrow = new ImageBox(arrowsThreeBox->lastBox, new Image("bell_arrow_right_W_M"));
        rightArrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        rightArrow->setMinimumSizeToFitImage();
        rightArrow->setVisible(true);
        rightArrow->setEdges(RectangleEdge::None);
        arrowsThreeBox->lastBox->setMinimumSize(rightArrow->widgetMinimumArea.w, rightArrow->widgetMinimumArea.h);
    }

    void BellBaseLayout::setArrowVisible(Arrow arrow, bool isVisible)
    {
        auto item = arrow == Arrow::Left ? leftArrow : rightArrow;
        if (item != nullptr) {
            item->setVisible(isVisible);
        }
    }

    void BellBaseLayout::setMinMaxArrowsVisibility(bool minCondition, bool maxCondition)
    {
        setArrowVisible(BellBaseLayout::Arrow::Left, !minCondition);
        setArrowVisible(BellBaseLayout::Arrow::Right, !maxCondition);
        if (arrowsThreeBox != nullptr) {
            if (arrowsThreeBox->firstBox != nullptr) {
                arrowsThreeBox->firstBox->resizeItems();
            }
            if (arrowsThreeBox->lastBox != nullptr) {
                arrowsThreeBox->lastBox->resizeItems();
            }
        }
    }
} // namespace gui
