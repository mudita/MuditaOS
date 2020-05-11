/*
 * BoundingBox.h
 *
 *  Created on: 4 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_
#define MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_

#include "Axes.hpp"
#include <cstdint>
#include <string>

namespace gui
{

    class BoundingBox
    {
      public:
        struct
        {
            int16_t x = 0, y = 0;
            uint16_t w = 0, h = 0;
        };
        BoundingBox(int32_t x = 0, int32_t y = 0, uint32_t w = 0, uint32_t h = 0);
        virtual ~BoundingBox() = default;

        static bool intersect(const BoundingBox &box1, const BoundingBox &box2, BoundingBox &result);

        /// set x,y,w,h to zero
        void clear();
        /// get size in axis - in x get width in y get height
        uint16_t size(gui::Axis axis) const;
        /// get position in axis - in x get x, in y get y
        int16_t pos(gui::Axis axis) const;
        std::string str() const;
        /// logical sum of bounding box by another bounding box values
        void sum(const BoundingBox &box);
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_BOUNDINGBOX_HPP_ */
