/*
 * Margins.cpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#include "Margins.hpp"
#include <core/BoundingBox.hpp>

namespace gui
{

    Margins::Margins() : left{0}, top{0}, right{0}, bottom{0}
    {}

    Margins::Margins(const short left, const short top, const short right, const short bottom)
        : left{left}, top{top}, right{right}, bottom{bottom}
    {}

    Margins::~Margins()
    {}

    short Margins::getSumInAxis(gui::Axis axis) const
    {
        switch (axis) {
        case Axis::X:
            return (left + right);
        case Axis::Y:
            return (top + bottom);
        default:
            return 0;
        }
    }

    short Margins::getMarginInAxis(Axis axis, MarginInAxis pos) const
    {
        switch (axis) {

        case Axis::X:

            switch (pos) {

            case MarginInAxis::First:
                return left;

            case MarginInAxis::Second:
                return right;

            default:
                return 0;
            }

        case Axis::Y:

            switch (pos) {

            case MarginInAxis::First:
                return top;

            case MarginInAxis::Second:
                return bottom;

            default:
                return 0;
            }

        default:
            return 0;
        }
    }

    bool Margins::operator==(const Margins &margins)
    {
        return !(left != margins.left || top != margins.top || right != margins.right || bottom != margins.bottom);
    }

    bool Margins::operator!=(const Margins &margins)
    {
        return (left != margins.left || top != margins.top || right != margins.right || bottom != margins.bottom);
    }

} /* namespace gui */
