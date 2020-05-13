#pragma once

#include "Axes.hpp"
#include <cstdint>
#include <string>

namespace gui
{

    /// defines both: size & position
    /// size is always in:     { zero_size < max_size }
    /// position is always in: { min_position < zero_position < max_position }
    class BoundingBox
    {
      public:
        static const uint16_t zero_size;
        static const uint16_t max_size;
        static const uint16_t min_size;

        static const int16_t zero_position;
        static const int16_t max_position;
        static const int16_t min_position;

        struct
        {
            int16_t x = zero_position, y = zero_position;
            uint16_t w = zero_size, h = zero_size;
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
