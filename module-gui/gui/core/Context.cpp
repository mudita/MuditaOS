// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ios>
#include <cstring>

#include "BoundingBox.hpp"
#include "Context.hpp"

namespace gui
{

    Context::Context(uint16_t width, uint16_t height) : w{width}, h{height}, data(std::make_unique<uint8_t[]>(w * h))
    {
        memset(data.get(), 15, w * h);
    }

    Context::Context(Size size) : Context(size.width, size.height)
    {}

    Context &Context::operator=(Context &&right)
    {
        this->x    = right.x;
        this->y    = right.y;
        this->w    = right.w;
        this->h    = right.h;
        this->data = std::move(right.data);

        right.x = 0;
        right.y = 0;
        right.w = 0;
        right.h = 0;

        return *this;
    }

    Context::Context(Context &&right)
    {
        operator=(std::move(right));
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
            uint32_t sourceOffset = resultBox.y * w + resultBox.x;
            uint32_t destOffset   = (resultBox.y - gy) * width + (resultBox.x - gx);
            for (int32_t h = 0; h < resultBox.h; h++) {
                memcpy(retContext->data.get() + destOffset, data.get() + sourceOffset, resultBox.w);
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
            uint32_t sourceOffset = (resultBox.y - iy) * context->w + (resultBox.x - ix);
            uint32_t destOffset   = (resultBox.y) * w + (resultBox.x);
            for (int32_t h = 0; h < resultBox.h; h++) {
                memcpy(data.get() + destOffset, context->data.get() + sourceOffset, resultBox.w);
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
            uint32_t sourceOffset = (resultBox.y - iy - yBoxOffset) * context->w + (resultBox.x - ix - xBoxOffset);
            uint32_t destOffset   = (resultBox.y) * w + (resultBox.x);
            for (int32_t h = 0; h < resultBox.h; h++) {
                memcpy(data.get() + destOffset, context->data.get() + sourceOffset, resultBox.w);
                sourceOffset += context->w;
                destOffset += w;
            }
        }
    }

    void Context::fill(uint8_t colour)
    {
        if (data.get() != nullptr) {
            memset(data.get(), colour, bufferMemorySize());
        }
    }

} /* namespace gui */
