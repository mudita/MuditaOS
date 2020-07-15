/*
 * BottomBar.cpp
 *
 *  Created on: 13 mar 2019
 *      Author: robert
 */
#include "BottomBar.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "utf8/UTF8.hpp"
#include <Style.hpp>
#include <log/log.hpp>

namespace gui
{

    BottomBar::BottomBar()
    {

        Margins margins{20, 0, 20, 0};
        left   = prepareLabel(Side::LEFT);
        center = prepareLabel(Side::CENTER);
        right  = prepareLabel(Side::RIGHT);

        left->setMargins(margins);
        center->setMargins(margins);
        right->setMargins(margins);

        addWidget(left);
        addWidget(center);
        addWidget(right);

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(480, 50);
    }
    BottomBar::BottomBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        Margins margins{20, 0, 20, 0};
        left   = prepareLabel(Side::LEFT);
        center = prepareLabel(Side::CENTER);
        right  = prepareLabel(Side::RIGHT);

        left->setMargins(margins);
        center->setMargins(margins);
        right->setMargins(margins);

        addWidget(left);
        addWidget(center);
        addWidget(right);

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(480, 50);
        updateDrawArea();
    }
    BottomBar::~BottomBar()
    {}

    gui::Label *BottomBar::prepareLabel(BottomBar::Side side)
    {
        Label *label = new Label(this, 0, 0, 0, 0);
        label->setBorderColor(Color{15, 15});
        switch (side) {
        case Side::LEFT:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::medium);
            break;
        case Side::CENTER:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::bold);
            break;
        case Side::RIGHT:
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            label->setFont(style::footer::font::medium);
            break;
        };

        label->setFilled(false);

        return label;
    }

    Label *BottomBar::getSide(BottomBar::Side side)
    {
        switch (side) {
        case Side::LEFT:
            return left;
        case Side::CENTER:
            return center;
        case Side::RIGHT:
            return right;
        default:
            LOG_ERROR("shall never got here - just to ommit warning");
            return right;
        };
        return nullptr;
    }

    void BottomBar::setActive(BottomBar::Side side, bool active)
    {
        getSide(side)->setVisible(active);
    }

    bool BottomBar::isActive(BottomBar::Side side)
    {
        return getSide(side)->visible;
    }

    void BottomBar::setText(BottomBar::Side side, const UTF8 &str, bool active)
    {
        getSide(side)->setText(str);
        setActive(side, active);
    }

    UTF8 BottomBar::getText(BottomBar::Side side)
    {
        return getSide(side)->getText();
    }

    bool BottomBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        left->setSize(newDim.w, newDim.h);
        center->setSize(newDim.w, newDim.h);
        right->setSize(newDim.w, newDim.h);
        return true;
    }

    void BottomBar::store()
    {
        store(Side::LEFT);
        store(Side::CENTER);
        store(Side::RIGHT);
    }

    void BottomBar::store(Side side)
    {
        auto &el = cache.get(side);
        if (!el.stored) {
            el.text      = getSide(side)->getText();
            el.isVisible = getSide(side)->visible;
            el.stored    = true;
        }
    }

    void BottomBar::restore()
    {
        restore(Side::LEFT);
        restore(Side::CENTER);
        restore(Side::RIGHT);
    }

    void BottomBar::restore(BottomBar::Side side)
    {
        auto &el = cache.get(side);
        if (el.stored) {
            setText(side, el.text, el.isVisible);
            el.stored = false;
        }
    }

} /* namespace gui */
