// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <type_traits>
#include <memory>

namespace gui
{
    class FrameBuffer;
    class FrameBufferPool;

    /// frame buffer shared memory facade to operate on
    /// instead creating/destroying huge (480x600) elements on memory
    class FrameBufferFlyweight
    {
      private:
        struct
        {
            FrameBuffer &fb;         /// framebuffer managed
            FrameBufferPool &origin; /// pool managing fb
        };                           // IntrinsticState

      public:
        FrameBufferFlyweight(FrameBuffer &fb, FrameBufferPool &origin);
        /// TODO can I set it free without destructor?
        /// maybe... use fb & origin as memento
        ~FrameBufferFlyweight();
        [[nodiscard]] auto empty() noexcept -> bool;
        [[nodiscard]] auto get() const noexcept -> const FrameBuffer &;
    };
} // namespace gui

/// FrableBufferFlyweight should be trivially copyable, it's meant to be passed
static_assert(std::is_trivially_copyable<gui::FrameBufferFlyweight>().value,
              "FrameBufferFlyweight should be trivially copyable, please see docs");
static_assert(std::is_trivially_move_assignable<gui::FrameBufferFlyweight>().value,
              "FrameBufferFlyweight should be trivially copyable, please see docs");
static_assert(std::is_trivially_move_constructible<gui::FrameBufferFlyweight>().value,
              "FrameBufferFlyweight should be trivially copyable, please see docs");
