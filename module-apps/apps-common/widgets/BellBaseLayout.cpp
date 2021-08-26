// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBaseLayout.hpp"
#include "Style.hpp"

namespace gui
{
    BellBaseLayout::BellBaseLayout(Item *parent,
                                   Position x,
                                   Position y,
                                   Length w,
                                   Length h,
                                   bool withSideArrows,
                                   style::bell_base_layout::ParentType type)
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
        centerBox->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);

        lastBox = new VBox(this);
        lastBox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        lastBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        lastBox->setEdges(RectangleEdge::None);
        lastBox->activeItem = false;

        if (type == style::bell_base_layout::ParentType::SideListView) {
            lastBox->setMargins(Margins(0, 0, 0, style::bell_base_layout::outer_layout_margin));
        }

        resizeItems();

        if (withSideArrows) {
            addSideArrows();
        }
    }

    Item *BellBaseLayout::getCenterBox() const noexcept
    {
        if (centerThreeBox != nullptr) {
            return centerThreeBox->centerBox;
        }
        return centerBox;
    }

    void BellBaseLayout::resizeCenter()
    {
        centerBox->resizeItems();
        if (centerThreeBox != nullptr) {
            centerThreeBox->resizeItems();
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
        centerThreeBox = new HThreeBox<HBox, HBox, HBox>(centerBox);
        centerThreeBox->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        centerThreeBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        centerThreeBox->setEdges(RectangleEdge::None);

        centerThreeBox->firstBox = new HBox(centerThreeBox);
        centerThreeBox->firstBox->setAlignment(Alignment(Alignment::Vertical::Center));
        centerThreeBox->firstBox->setEdges(RectangleEdge::None);
        centerThreeBox->firstBox->activeItem = false;

        leftArrow = new ImageBox(centerThreeBox->firstBox, 0, 0, 0, 0, new Image("bell_arrow_left"));
        leftArrow->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
        leftArrow->setMinimumSizeToFitImage();
        leftArrow->setVisible(true);
        leftArrow->setEdges(RectangleEdge::None);
        centerThreeBox->firstBox->setMinimumSize(leftArrow->widgetMinimumArea.w, leftArrow->widgetMinimumArea.h);

        centerThreeBox->centerBox = new HBox(centerThreeBox);
        centerThreeBox->centerBox->setEdges(RectangleEdge::None);
        centerThreeBox->centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        centerThreeBox->centerBox->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);

        centerThreeBox->lastBox = new HBox(centerThreeBox);
        centerThreeBox->lastBox->setAlignment(Alignment(Alignment::Vertical::Center));
        centerThreeBox->lastBox->setEdges(RectangleEdge::None);
        centerThreeBox->lastBox->activeItem = false;

        rightArrow = new ImageBox(centerThreeBox->lastBox, 0, 0, 0, 0, new Image("bell_arrow_right"));
        rightArrow->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        rightArrow->setMargins(Margins(0, 0, 0, 0));
        rightArrow->setMinimumSizeToFitImage();
        rightArrow->setVisible(true);
        rightArrow->setEdges(RectangleEdge::None);
        centerThreeBox->lastBox->setMinimumSize(rightArrow->widgetMinimumArea.w, rightArrow->widgetMinimumArea.h);
    }

    void BellBaseLayout::setArrowVisible(Arrow arrow, bool isVisible)
    {
        auto item = arrow == Arrow::Left ? leftArrow : rightArrow;
        if (item != nullptr) {
            item->setVisible(isVisible);
        }
    }
} // namespace gui
