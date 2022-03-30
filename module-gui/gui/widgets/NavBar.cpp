// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NavBar.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "utf8/UTF8.hpp"
#include <Style.hpp>
#include <log/log.hpp>

namespace gui::nav_bar
{
    NavBar::NavBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : HThreeBox<HBox, HBox, HBox>(parent, x, y, w, h)
    {
        setEdges(RectangleEdge::None);

        firstBox = new HBox(this);
        firstBox->setMinimumHeight(h);
        firstBox->setAlignment(Alignment(Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        firstBox->setMargins({style::nav_bar::left_margin, 0, 0, 0});
        firstBox->setEdges(RectangleEdge::None);

        centerBox = new HBox(this);
        centerBox->setEdges(RectangleEdge::None);
        centerBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        centerBox->setMaximumSize(w, h);

        lastBox = new HBox(this);
        lastBox->setMinimumHeight(h);
        lastBox->setAlignment(Alignment(Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        lastBox->setMargins({0, 0, style::nav_bar::right_margin, 0});
        lastBox->setEdges(RectangleEdge::None);

        createLabels();

        resizeItems();
    }

    void NavBar::createLabels()
    {
        left = new Label(firstBox);
        left->setAlignment(Alignment(Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        left->setPadding({0, 0, 0, style::nav_bar::bottom_padding});
        left->setMinimumHeight(widgetArea.h);
        left->setFont(style::window::font::smallbold);
        left->setTextEllipsisType(TextEllipsis::Right);
        left->setVisible(true);
        left->setEdges(RectangleEdge::None);

        center = new Label(centerBox);
        center->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        center->setPadding({0, 0, 0, style::nav_bar::bottom_padding});
        center->setMinimumHeight(widgetArea.h);
        center->setMaximumWidth(widgetArea.w);
        center->setFont(style::window::font::smallbold);
        center->setTextEllipsisType(TextEllipsis::Right);
        center->setVisible(true);
        center->setEdges(RectangleEdge::None);

        right = new Label(lastBox);
        right->setAlignment(Alignment(Alignment::Horizontal::Right, gui::Alignment::Vertical::Bottom));
        right->setPadding({0, 0, 0, style::nav_bar::bottom_padding});
        right->setMinimumHeight(widgetArea.h);
        right->setFont(style::window::font::smallbold);
        right->setTextEllipsisType(TextEllipsis::Right);
        right->setVisible(true);
        right->setEdges(RectangleEdge::None);
    }

    Label *NavBar::getSide(nav_bar::Side side)
    {
        switch (side) {
        case Side::Left:
            return left;
        case Side::Center:
            return center;
        case Side::Right:
            return right;
        };
        return nullptr;
    }

    bool NavBar::checkSideOccupied(nav_bar::Side side)
    {
        return getSide(side)->visible && !getSide(side)->getText().empty();
    }

    Length NavBar::outerSideUsedWidth(nav_bar::Side side)
    {
        return getSide(side)->visible ? getSide(side)->getTextNeedSpace() : 0;
    }

    void NavBar::setLeftSideWidth(Length width)
    {
        left->setMinimumWidth(width);
        firstBox->setMinimumWidth(width);
        firstBox->setMaximumWidth(width);
    }

    void NavBar::setRightSideWidth(Length width)
    {
        right->setMinimumWidth(width);
        lastBox->setMinimumWidth(width);
        lastBox->setMaximumWidth(width);
    }

    void NavBar::applyOuterSidesWidth(nav_bar::Side side, const UTF8 &str)
    {
        Length widthToSet = 0U;
        if (side == Side::Left) {
            widthToSet = std::max(outerSideUsedWidth(Side::Right), left->visible ? left->getTextNeedSpace(str) : 0);
        }
        else if (side == Side::Right) {
            widthToSet = std::max(outerSideUsedWidth(Side::Left), right->visible ? right->getTextNeedSpace(str) : 0);
        }
        else {
            widthToSet = std::max(outerSideUsedWidth(Side::Left), outerSideUsedWidth(Side::Right));
        }

        // If other side active set maximum for side to print into
        auto widthReduced = false;
        if ((side == Side::Left && !str.empty() &&
             (checkSideOccupied(Side::Right) || checkSideOccupied(Side::Center))) ||
            (side == Side::Right && !str.empty() &&
             (checkSideOccupied(Side::Left) || checkSideOccupied(Side::Center))) ||
            (side == Side::Center && !str.empty() &&
             (checkSideOccupied(Side::Left) || checkSideOccupied(Side::Right)))) {

            widthReduced = true;
            widthToSet =
                std::min(widthToSet, (widgetArea.w - (style::nav_bar::left_margin + style::nav_bar::right_margin)) / 3);
        }

        if (widthReduced) {
            setLeftSideWidth(widthToSet);
            setRightSideWidth(widthToSet);
        }
        else if (side == Side::Left && !str.empty()) {
            setLeftSideWidth(widthToSet);
            setRightSideWidth(0);
        }
        else if (side == Side::Right && !str.empty()) {
            setRightSideWidth(widthToSet);
            setLeftSideWidth(0);
        }
    }

    void NavBar::setActive(nav_bar::Side side, bool active)
    {
        auto selectedSide = getSide(side);

        selectedSide->setVisible(active);
        applyOuterSidesWidth(side, selectedSide->getText());
        selectedSide->informContentChanged();
    }

    bool NavBar::isActive(nav_bar::Side side)
    {
        return getSide(side)->visible;
    }

    void NavBar::setText(nav_bar::Side side, const UTF8 &str, bool active)
    {
        auto selectedSide = getSide(side);
        selectedSide->setVisible(active);

        applyOuterSidesWidth(side, str);
        selectedSide->setText(str);
        selectedSide->informContentChanged();
    }

    UTF8 NavBar::getText(nav_bar::Side side)
    {
        return getSide(side)->getText();
    }

    void NavBar::store()
    {
        store(Side::Left);
        store(Side::Center);
        store(Side::Right);
    }

    void NavBar::store(Side side)
    {
        auto &el = cache.get(side);
        if (!el.stored) {
            el.text      = getSide(side)->getText();
            el.isVisible = getSide(side)->visible;
            el.stored    = true;
        }
    }

    void NavBar::restore()
    {
        restore(Side::Left);
        restore(Side::Center);
        restore(Side::Right);
    }

    void NavBar::restore(nav_bar::Side side)
    {
        auto &el = cache.get(side);
        if (el.stored) {
            setText(side, el.text, el.isVisible);
            el.stored = false;
        }
    }

    void NavBar::setFont(Side side, const UTF8 &fontName)
    {
        switch (side) {
        case Side::Left:
            left->setFont(fontName);
            break;
        case Side::Center:
            center->setFont(fontName);
            break;
        case Side::Right:
            right->setFont(fontName);
            break;
        }
    }

    void NavBar::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }

} // namespace gui::nav_bar
