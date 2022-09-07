// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Axes.hpp"
#include "gui/Common.hpp"
#include <cstdint>
#include <limits>
#include <string>

namespace gui
{

    /// defines both: size & position
    /// size is always in:     { zero_size < max_size }
    /// position is always in: { min_position < zero_position < max_position }
    class BoundingBox
    {
      public:
        static constexpr Length zero_size = 0;
        static constexpr Length max_size  = std::numeric_limits<Length>().max();
        static constexpr Length min_size  = std::numeric_limits<Length>().min();

        static constexpr Position zero_position = 0;
        static constexpr Position max_position  = std::numeric_limits<Position>().max();
        static constexpr Position min_position  = std::numeric_limits<Position>().min();

        struct
        {
            Position x = zero_position, y = zero_position;
            Length w = zero_size, h = zero_size;
        };
        BoundingBox() = default;
        BoundingBox(Position x, Position y, Length w, Length h);

        static bool intersect(const BoundingBox &box1, const BoundingBox &box2, BoundingBox &result);

        /// set x,y,w,h to zero
        void clear();
        /// get size in axis - in x get width in y get height
        Length size(gui::Axis axis) const;
        /// get position in axis - in x get x, in y get y
        Position pos(gui::Axis axis) const;
        std::string str() const;
        /// logical sum of bounding box by another bounding box values
        // NOTE: This function only increases the size, position is intact so this is not really a sum
        void sum(const BoundingBox &box);
        bool operator==(const BoundingBox &box) const;
        bool operator!=(const BoundingBox &box) const;
    };

} /* namespace gui */
