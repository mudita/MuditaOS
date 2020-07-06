#include "BoundingBox.hpp"
#include <limits>
#include <sstream>
#include <limits>

namespace gui
{
    const uint16_t BoundingBox::zero_size = 0;
    const uint16_t BoundingBox::max_size  = std::numeric_limits<uint16_t>().max();
    const uint16_t BoundingBox::min_size  = std::numeric_limits<uint16_t>().min();

    const int16_t BoundingBox::zero_position = 0;
    const int16_t BoundingBox::max_position  = std::numeric_limits<int16_t>().max();
    const int16_t BoundingBox::min_position  = std::numeric_limits<int16_t>().min();

    BoundingBox::BoundingBox(int32_t x, int32_t y, uint32_t w, uint32_t h)
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
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }

    uint16_t BoundingBox::size(gui::Axis axis) const
    {
        if (axis == Axis::X)
            return w;
        else
            return h;
    }

    int16_t BoundingBox::pos(gui::Axis axis) const
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

    void BoundingBox::sum(const BoundingBox &box)
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
