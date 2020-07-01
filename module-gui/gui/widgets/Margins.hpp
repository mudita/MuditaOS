/*
 * Margins.h
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_
#define MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_

#include "core/Axes.hpp"

namespace gui
{
    enum class MarginInAxis
    {
        First,
        Second
    };

    class Margins
    {
      public:
        short left, top, right, bottom;

        Margins();
        Margins(const short left, const short top, const short right, const short bottom);
        virtual ~Margins();

        [[nodiscard]] short getSumInAxis(Axis axis) const;
        [[nodiscard]] short getMarginInAxis(Axis axis, MarginInAxis pos) const;
        bool operator==(const Margins &margins);
        bool operator!=(const Margins &margins);
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_ */
