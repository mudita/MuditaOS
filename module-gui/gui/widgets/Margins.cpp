// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

    Position Margins::getSumInAxis(gui::Axis axis) const
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

    Position Margins::getMarginInAxis(Axis axis, MarginInAxis pos) const
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
