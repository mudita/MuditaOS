/*
 * Alignment.hpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_
#define MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_

#include <cstdint>

namespace gui
{

    class Alignment
    {
      public:
        enum : uint32_t
        {
            ALIGN_VERTICAL_CENTER   = 1 << 0,
            ALIGN_VERTICAL_TOP      = 1 << 1,
            ALIGN_VERTICAL_BOTTOM   = 1 << 2,
            ALIGN_HORIZONTAL_CENTER = 1 << 3,
            ALIGN_HORIZONTAL_LEFT   = 1 << 4,
            ALIGN_HORIZONTAL_RIGHT  = 1 << 5,
        };

        union
        {
            uint32_t alignment;
            struct
            {
                uint32_t vertical_center : 1, vertical_top : 1, vertical_bottom : 1, horizontal_center : 1,
                    horizontal_left : 1, horizontal_right : 1;
            };
        };

        Alignment();
        Alignment(const uint32_t valH, const uint32_t valV);
        Alignment(const uint32_t align);
        virtual ~Alignment();
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_ */
