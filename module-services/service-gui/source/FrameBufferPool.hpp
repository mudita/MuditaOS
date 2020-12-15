// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Common.hpp"
#include <memory>
#include "FrameBufferFlyweight.hpp"

namespace gui
{

    class FrameBuffer;

    /// on stack/heap store for framebuffers
    /// one shall always use facate to access frame buffer
    class FrameBufferPool
    {
        friend FrameBufferFlyweight;

        enum class State
        {
            Ready,
            Taken
        };

        static constexpr int MaxFB = 4; /// maximal amount of frame buffers
        std::array<std::unique_ptr<FrameBuffer>, MaxFB>
            fbs; /// array of frame buffers stored in system
                 /// TODO store Frame buffer state - taken & free
                 /// according to Returned FrameBufferFlyweight (and destroyed)

        void setFrameBufferTaken(FrameBufferFlyweight &fb);
        void setFrameBufferFree(FrameBufferFlyweight &fb);
        void setFrameBufferState(FrameBufferFlyweight &fb, State st);
        void setState(State st);

      public:
        FrameBufferPool(Size size);
        [[nodiscard]] auto emptyFlyweight() noexcept -> FrameBufferFlyweight;
        /// returns first available frame buffer, or emptyFlyweight
        /// TODO Add rest :D
        [[nodiscard]] auto getFrameBuffer() noexcept -> FrameBufferFlyweight;
    };
} // namespace gui
