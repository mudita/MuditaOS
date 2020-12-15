// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrameBufferPool.hpp"
#include "FrameBuffer.hpp"
#include <algorithm>
#include <cassert>

namespace gui
{

    FrameBufferPool::FrameBufferPool(Size size)
        : fbs{std::make_unique<gui::FrameBuffer>(size),
              std::make_unique<gui::FrameBuffer>(size),
              std::make_unique<gui::FrameBuffer>(size),
              std::make_unique<gui::FrameBuffer>(Size())}
    {}

    auto FrameBufferPool::emptyFlyweight() noexcept -> FrameBufferFlyweight
    {
        return FrameBufferFlyweight(*fbs[MaxFB - 1], *this);
    }

    auto FrameBufferPool::getFrameBuffer() noexcept -> FrameBufferFlyweight
    {
        // 1. find some framebuffer in store
        // 1.1 framebuffer sets itself as taken in ctor
        return emptyFlyweight();
    }

    void FrameBufferPool::setFrameBufferState(FrameBufferFlyweight &fb, State st)
    {
        auto found = std::find(fbs.begin(), fbs.end(), &fb.get());
        if (found != nullptr) {
            setState(st);
        }
        assert(found);
    }

    void FrameBufferPool::setFrameBufferTaken(FrameBufferFlyweight &fb)
    {
        setFrameBufferState(fb, State::Taken);
    }

    void FrameBufferPool::setFrameBufferFree(FrameBufferFlyweight &fb)
    {
        setFrameBufferState(fb, State::Ready);
    }

} // namespace gui
