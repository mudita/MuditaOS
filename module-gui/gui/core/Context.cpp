// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Context.cpp
 *
 *  Created on: 6 maj 2019
 *      Author: robert
 */

#include <ios>
#include <cstring>

#include "BoundingBox.hpp"
#include "Context.hpp"

namespace gui
{

    Context::Context() : x{0}, y{0}, w{0}, h{0}, data{nullptr}
    {}

    Context::Context(uint16_t width, uint16_t height) : x{0}, y{0}, w{width}, h{height}, data(new uint8_t[w * h])
    {
        memset(data, 15, w * h);
    }

    Context::~Context()
    {
        if (data != nullptr)
            delete[] data;
    }

    Context *Context::get(int16_t gx, int16_t gy, uint16_t width, uint16_t height)
    {
        Context *retContext = new Context(width, height);

        retContext->x = gx;
        retContext->y = gy;

        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox newBox     = BoundingBox(gx, gy, width, height);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, newBox, resultBox)) {
            Length sourceOffset = resultBox.y * w + resultBox.x;
            Length destOffset   = (resultBox.y - gy) * width + (resultBox.x - gx);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(retContext->data + destOffset, data + sourceOffset, resultBox.w);
                sourceOffset += w;
                destOffset += width;
            }
        }
        // else just return context filled with white colour.

        return retContext;
    }

    void Context::insert(int16_t ix, int16_t iy, Context *context)
    {
        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox insertBox  = BoundingBox(ix, iy, context->w, context->h);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, insertBox, resultBox)) {
            Length sourceOffset = (resultBox.y - iy) * context->w + (resultBox.x - ix);
            Length destOffset   = (resultBox.y) * w + (resultBox.x);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(data + destOffset, context->data + sourceOffset, resultBox.w);
                sourceOffset += context->w;
                destOffset += w;
            }
        }
    }

    void Context::insertArea(
        int16_t ix, int16_t iy, int16_t iareaX, int16_t iareaY, int16_t iareaW, int16_t iareaH, Context *context)
    {
        // create bounding boxes for the current context and return context
        BoundingBox currentBox = BoundingBox(0, 0, w, h);
        BoundingBox insertBox  = BoundingBox(ix, iy, iareaW, iareaH);
        BoundingBox resultBox;

        // if boxes overlap copy part defined by result from current context to the new context.
        if (BoundingBox::intersect(currentBox, insertBox, resultBox)) {
            int16_t xBoxOffset = 0;
            int16_t yBoxOffset = 0;
            if (iareaX < 0)
                xBoxOffset = iareaX;
            if (iareaY < 0)
                yBoxOffset = iareaY;
            Length sourceOffset = (resultBox.y - iy - yBoxOffset) * context->w + (resultBox.x - ix - xBoxOffset);
            Length destOffset   = (resultBox.y) * w + (resultBox.x);
            for (Length h = 0; h < resultBox.h; h++) {
                memcpy(data + destOffset, context->data + sourceOffset, resultBox.w);
                sourceOffset += context->w;
                destOffset += w;
            }
        }
    }

    void Context::fill(uint8_t colour)
    {
        if (data) {
            memset(data, colour, w * h);
        }
        //	uint32_t size = 480*600;
        //	memset( data, colour, size );
    }

    std::ostream &operator<<(std::ostream &out, const Context &c)
    {
        out << "x:" << c.x << "y:" << c.y << "w:" << c.w << "h:" << c.h << std::endl;

        uint32_t offset = 0;
        for (uint32_t y = 0; y < c.h; y++) {
            for (uint32_t x = 0; x < c.w; x++) {
                uint32_t value = *(c.data + offset);
                std::cout << std::hex << value;
                offset++;
            }
            std::cout << std::endl;
        }
        return out;
    }

} /* namespace gui */
