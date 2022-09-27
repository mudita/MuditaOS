// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BoundingBox.hpp"
#include <limits>
#include <sstream>
#include <limits>
#include <module-gui/gui/Common.hpp>

namespace gui
{
    BoundingBox::BoundingBox(Position x, Position y, Length w, Length h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    bool BoundingBox::intersect(const BoundingBox &box1, const BoundingBox &box2, BoundingBox &result)
    {
        const BoundingBox *l;
        const BoundingBox *r;

        if (box1.x < box2.x) {
            l = &box1;
            r = &box2;
        }
        else {
            l = &box2;
            r = &box1;
        }

        int w = l->x + l->w - r->x;
        if (w <= 0) {
            result.clear();
            return false;
        }
        // most left bbox overlap right box entirely
        if ((l->x + l->w) >= (r->x + r->w)) {
            w = r->w;
        }
        result.w = w;
        result.x = r->x;

        // vertical check
        // select bbox that is higher
        const BoundingBox *u;
        if (box1.y < box2.y) {
            l = &box1;
            u = &box2;
        }
        else {
            l = &box2;
            u = &box1;
        }

        int h = l->y + l->h - u->y;

        if (h <= 0) {
            result.clear();
            return false;
        }

        // most lower bbox overlap upper box entirely
        if ((l->y + l->h) >= (u->y + u->h)) {
            h = u->h;
        }
        result.y = u->y;
        result.h = h;

        return true;
    }

    void BoundingBox::clear()
    {
        x = zero_position;
        y = zero_position;
        w = zero_size;
        h = zero_size;
    }

    Length BoundingBox::size(gui::Axis axis) const
    {
        if (axis == Axis::X)
            return w;
        else
            return h;
    }

    Position BoundingBox::pos(gui::Axis axis) const
    {
        if (axis == Axis::X)
            return x;
        else
            return y;
    }

    std::string BoundingBox::str() const
    {
        std::stringstream ss;
        ss << "{";
        ss << "x: " << x << ", y: " << y << ", w: " << w << ", h: " << h;
        ss << "}";
        return ss.str();
    }

    void BoundingBox::expandSize(const BoundingBox &box)
    {
        w = box.w > w ? box.w : w;
        h = box.h > h ? box.h : h;
    }

    bool BoundingBox::operator==(const BoundingBox &box) const
    {
        return !(x != box.x || y != box.y || w != box.w || h != box.h);
    }

    bool BoundingBox::operator!=(const BoundingBox &box) const
    {
        return (x != box.x || y != box.y || w != box.w || h != box.h);
    }

} /* namespace gui */
