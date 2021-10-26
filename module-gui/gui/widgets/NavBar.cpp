// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NavBar.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "utf8/UTF8.hpp"
#include <Style.hpp>
#include <log/log.hpp>

namespace gui::nav_bar
{
    NavBar::NavBar()
    {

        Padding margins{style::window::navBar::leftMargin, 0, style::window::navBar::rightMargin, 0};
        left   = prepareLabel(Side::Left);
        center = prepareLabel(Side::Center);
        right  = prepareLabel(Side::Right);

        left->setPadding(margins);
        center->setPadding(margins);
        right->setPadding(margins);

        addWidget(left);
        addWidget(center);
        addWidget(right);

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(style::window::navBar::w, style::window::navBar::h);
    }
    NavBar::NavBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        Padding margins{style::window::navBar::leftMargin, 0, style::window::navBar::rightMargin, 0};
        left   = prepareLabel(Side::Left);
        center = prepareLabel(Side::Center);
        right  = prepareLabel(Side::Right);

        left->setPadding(margins);
        center->setPadding(margins);
        right->setPadding(margins);

        addWidget(left);
        addWidget(center);
        addWidget(right);

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(style::window::navBar::w, style::window::navBar::h);
        updateDrawArea();
    }
    NavBar::~NavBar()
    {}

    gui::Label *NavBar::prepareLabel(nav_bar::Side side)
    {
        Label *label = new Label(this, 0, 0, 0, 0);
        label->setBorderColor(Color{15, 15});
        switch (side) {
        case Side::Left:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::medium);
            break;
        case Side::Center:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::bold);
            break;
        case Side::Right:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::medium);
            break;
        };

        label->setFilled(false);

        return label;
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

    void NavBar::setActive(nav_bar::Side side, bool active)
    {
        getSide(side)->setVisible(active);
    }

    bool NavBar::isActive(nav_bar::Side side)
    {
        return getSide(side)->visible;
    }

    void NavBar::setText(nav_bar::Side side, const UTF8 &str, bool active)
    {
        getSide(side)->setText(str);
        setActive(side, active);
    }

    UTF8 NavBar::getText(nav_bar::Side side)
    {
        return getSide(side)->getText();
    }

    bool NavBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        left->setSize(newDim.w, newDim.h);
        center->setSize(newDim.w, newDim.h);
        right->setSize(newDim.w, newDim.h);
        return true;
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
