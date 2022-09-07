// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Context.hpp
 *
 *  Created on: 6 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_CONTEXT_HPP_
#define GUI_CORE_CONTEXT_HPP_

#include "BoundingBox.hpp"

#include "module-gui/gui/Common.hpp"

#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>

namespace gui
{
    class Context
    {
        static constexpr std::uint8_t clearColor = 15;

      public:
        Context() = default;
        Context(std::uint16_t width, std::uint16_t height);

        /**
         * @brief Creates new context using provided coordinates. If there is no common part Context filled with
         * clearColor is returned. The size of the returned context is defiend by parameters, so it may represent
         * area outside the original context.
         */
        Context get(std::int16_t gx, std::int16_t gy, std::uint16_t width, std::uint16_t height) const;
        /**
         * @brief Pastes provided context into current one. Overlapping content will be inserted into current context.
         */
        void insert(std::int16_t ix, std::int16_t iy, const Context &context);
        /**
         * @brief Pastes provided context into current one. Overlapping content will be inserted into current context.
         */
        void insertArea(std::int16_t ix,
                        std::int16_t iy,
                        std::int16_t iareaX,
                        std::int16_t iareaY,
                        std::int16_t iareaW,
                        std::int16_t iareaH,
                        const Context &context);

        /**
         * @brief Calculate regions of difference between contexts. Each bounding box covers the whole width of the
         * context. They are disjoint and sorted by y coordinate. The contexts has to have the same sizes.
         */
        static std::deque<BoundingBox> linesDiffs(const gui::Context &ctx1, const gui::Context &ctx2);

        /**
         * @brief Fills whole context with specified colour;
         */
        void fill(std::uint8_t colour);
        /**
         * @brief returns pointer to context's data;
         */
        inline const std::uint8_t *getData() const
        {
            return data.get();
        }
        inline std::uint8_t *getData()
        {
            return data.get();
        }
        inline std::uint16_t getW() const
        {
            return w;
        }
        inline std::uint16_t getH() const
        {
            return h;
        }
        inline BoundingBox getBoundingBox() const
        {
            return {0, 0, w, h};
        }

        inline std::uint8_t getPixel(const Point point, uint8_t defaultColor = clearColor) const
        {
            return hasPixel(point) ? data[point.y * w + point.x] : defaultColor;
        }

        inline bool hasPixel(const Point point) const noexcept
        {
            return (point.x >= 0 && static_cast<std::uint32_t>(point.x) < w) &&
                   (point.y >= 0 && static_cast<std::uint32_t>(point.y) < h);
        }

        friend std::ostream &operator<<(std::ostream &out, const Context &c);

      private:
        std::uint32_t w = 0;
        std::uint32_t h = 0;
        std::unique_ptr<uint8_t[]> data;
    };

} /* namespace gui */

#endif /* GUI_CORE_CONTEXT_HPP_ */
