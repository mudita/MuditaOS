// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrameBufferFlyweight.hpp"

#include "FrameBuffer.hpp"
#include "FrameBufferPool.hpp"

namespace gui
{

    FrameBufferFlyweight::FrameBufferFlyweight(FrameBuffer &fb, FrameBufferPool &origin) : fb(fb), origin(origin)
    {
        origin.setFrameBufferTaken(*this);
    }

    /// TODO can I set it free without destructor?
    /// maybe... use fb & origin as memento
    FrameBufferFlyweight::~FrameBufferFlyweight()
    {
        origin.setFrameBufferFree(*this);
    }

    auto FrameBufferFlyweight::empty() noexcept -> bool
    {
        return fb.empty();
    }

    [[nodiscard]] const FrameBuffer &FrameBufferFlyweight::get() const noexcept
    {
        return fb;
    }

} // namespace gui
