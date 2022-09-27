// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Context.cpp
 *
 *  Created on: 6 maj 2019
 *      Author: robert
 */

#include "Context.hpp"

#include <cassert>
#include <cstring>
#include <ios>
#include <vector>

namespace gui
{
    Context::Context(std::uint16_t width, std::uint16_t height) : w{width}, h{height}, data(new std::uint8_t[w * h])
    {
        memset(data.get(), clearColor, w * h);
    }

    Context Context::get(std::int16_t gx, std::int16_t gy, std::uint16_t width, std::uint16_t height) const
    {
        Context retContext = Context(width, height);

        // Copy the whole block if context is fully inside and covering the whole width
        if (gx == 0 && width == w && gy >= 0 && std::uint16_t(gy + height) <= h) {
            memcpy(retContext.data.get(), data.get() + gy * w, w * height);
            return retContext;
        }

        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox newBox     = BoundingBox(gx, gy, width, height);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, newBox, resultBox)) {
            Length sourceOffset = resultBox.y * w + resultBox.x;
            Length destOffset   = (resultBox.y - gy) * width + (resultBox.x - gx);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(retContext.data.get() + destOffset, data.get() + sourceOffset, resultBox.w);
                sourceOffset += w;
                destOffset += width;
            }
        }
        // else just return context filled with white colour.

        return retContext;
    }

    void Context::insert(std::int16_t ix, std::int16_t iy, const Context &context)
    {
        // Copy the whole block if context is fully inside and covering the whole width
        if (ix == 0 && context.w == w && iy >= 0 && std::uint16_t(iy + context.h) <= h) {
            memcpy(data.get() + iy * w, context.data.get(), w * context.h);
            return;
        }

        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox insertBox  = BoundingBox(ix, iy, context.w, context.h);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, insertBox, resultBox)) {
            Length sourceOffset = (resultBox.y - iy) * context.w + (resultBox.x - ix);
            Length destOffset   = (resultBox.y) * w + (resultBox.x);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(data.get() + destOffset, context.data.get() + sourceOffset, resultBox.w);
                sourceOffset += context.w;
                destOffset += w;
            }
        }
    }

    void Context::insertArea(std::int16_t ix,
                             std::int16_t iy,
                             std::int16_t iareaX,
                             std::int16_t iareaY,
                             std::int16_t iareaW,
                             std::int16_t iareaH,
                             const Context &context)
    {
        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox insertBox  = BoundingBox(ix, iy, iareaW, iareaH);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, insertBox, resultBox)) {
            std::int16_t xBoxOffset = 0;
            std::int16_t yBoxOffset = 0;
            if (iareaX < 0)
                xBoxOffset = iareaX;
            if (iareaY < 0)
                yBoxOffset = iareaY;
            Length sourceOffset = (resultBox.y - iy - yBoxOffset) * context.w + (resultBox.x - ix - xBoxOffset);
            Length destOffset   = (resultBox.y) * w + (resultBox.x);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(data.get() + destOffset, context.data.get() + sourceOffset, resultBox.w);
                sourceOffset += context.w;
                destOffset += w;
            }
        }
    }

    struct LRange
    {
        LRange(std::uint16_t begin, std::uint16_t end) : begin(begin), end(end)
        {}
        void expand(LRange const &other)
        {
            begin = std::min(begin, other.begin);
            end   = std::max(end, other.end);
        }
        static LRange inversed(std::uint16_t end)
        {
            return {end, 0};
        }
        std::uint16_t begin, end;
    };

    inline BoundingBox makeBoundingBox(const LRange &rangeX, const LRange &rangeY)
    {
        return {rangeX.begin,
                rangeY.begin,
                std::uint16_t(rangeX.end - rangeX.begin),
                std::uint16_t(rangeY.end - rangeY.begin)};
    }

    // Currently the algorithm only works properly for width divisible by 8 due to the use of 64b integers.
    std::deque<BoundingBox> gui::Context::linesDiffs(const gui::Context &ctx1, const gui::Context &ctx2)
    {
        using casted_t        = std::uint64_t;
        const std::uint16_t w = ctx1.getW();
        const std::uint16_t h = ctx1.getH();
        assert(w == ctx2.getW() && h == ctx2.getH() && w % 8 == 0);
        const std::uint16_t cw = w / sizeof(casted_t);
        const auto data1       = reinterpret_cast<const casted_t *>(ctx1.getData());
        const auto data2       = reinterpret_cast<const casted_t *>(ctx2.getData());

        std::deque<BoundingBox> result;
        LRange rangeY = LRange::inversed(h);
        for (std::uint16_t y = 0; y < h; ++y) {
            const auto begin1 = data1 + y * cw;
            const auto end1   = begin1 + cw;
            const auto begin2 = data2 + y * cw;
            if (std::mismatch(begin1, end1, begin2).first != end1) {
                if (rangeY.begin == h) { // diff pixels found first time
                    rangeY.begin = y;
                }
            }
            else {
                if (rangeY.begin != h) { // diff pixels found before
                    rangeY.end = y;
                    result.push_back(makeBoundingBox({0, w}, rangeY));
                    rangeY = LRange::inversed(h);
                }
            }
        }
        if (rangeY.begin != h) { // diff pixels found before
            rangeY.end = h;
            result.push_back(makeBoundingBox({0, w}, rangeY));
        }
        return result;
    }

    void Context::fill(std::uint8_t colour)
    {
        if (data) {
            memset(data.get(), colour, w * h);
        }
    }

    std::ostream &operator<<(std::ostream &out, const Context &c)
    {
        out << "w:" << c.w << "h:" << c.h << std::endl;

        std::uint32_t offset = 0;
        for (std::uint32_t y = 0; y < c.h; y++) {
            for (std::uint32_t x = 0; x < c.w; x++) {
                std::uint32_t value = *(c.data.get() + offset);
                std::cout << std::hex << value;
                offset++;
            }
            std::cout << std::endl;
        }
        return out;
    }

    std::string Context::toAsciiScaled(std::uint16_t scale) const
    {
        scale = std::max(std::uint16_t(1), std::min(scale, std::min(w, h)));

        const std::uint16_t pixelsPerChar = scale * scale;
        const std::uint16_t sw            = w / scale;
        const std::uint16_t sh            = h / scale;
        const std::uint8_t white          = 15;
        const char *chars                 = " .,-\"*^:;+=!?%#@";

        std::vector<std::uint16_t> accum(sw * sh, 0);
        for (std::uint16_t j = 0; j < h; ++j) {
            for (std::uint16_t i = 0; i < w; ++i) {
                const std::uint8_t c = std::min(data[j * w + i], white);
                const auto off       = (j / scale) * sw + (i / scale);
                accum[off] += c;
            }
        }

        const std::uint16_t sw_nl = sw + 1;
        std::string result(sw_nl * sh - 1, '\n'); // last new line is not needed

        for (std::uint16_t j = 0; j < sh; ++j) {
            for (std::uint16_t i = 0; i < sw; ++i) {
                const auto off_nl = j * sw_nl + i;
                const auto off    = j * sw + i;
                result[off_nl]    = chars[accum[off] / pixelsPerChar];
            }
        }

        return result;
    }

} /* namespace gui */
