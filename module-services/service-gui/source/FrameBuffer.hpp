// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/core/Context.hpp>

namespace gui
{
    class FrameBuffer
    {
      public:
        Size size;
        Context canvas;

        FrameBuffer()                      = delete;
        FrameBuffer(const FrameBuffer &fb) = delete;
        FrameBuffer(FrameBuffer &&)        = delete;
        FrameBuffer &operator=(const FrameBuffer &) = delete;
        FrameBuffer &operator=(FrameBuffer &&) = delete;
        virtual ~FrameBuffer()                 = default;

        FrameBuffer(Size size) : size(size), canvas(size)
        {}

        bool empty()
        {
            return size.isZero();
        }
    };

} // namespace gui
