// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TryLock.hpp"
#include "DrawData.hpp"
#include "FrameBufferFlyweight.hpp"
#include <service-eink/ImageData.hpp>
#include <functional>

namespace gui
{

    class FrameStack;

    using FrameProcessor =
        std::function<service::eink::ImageData(sgui::DrawData &data, gui::FrameBufferFlyweight &&fb)>;

    class DrawFrame
    {
        friend FrameStack;

      public:
        enum class State
        {
            Fresh,
            Processed
        };

      private:
        cpp_freertos::MutexStandard &processingLock;
        sgui::DrawData data;
        State state = State::Fresh;

      public:
        DrawFrame(cpp_freertos::MutexStandard &processingLock, sgui::DrawData &&data);

        auto processFrame(FrameProcessor processor, gui::FrameBufferFlyweight &&fb)
            -> std::optional<service::eink::ImageData>;

        [[nodiscard]] auto getState() const noexcept
        {
            return state;
        }

        [[nodiscard]] const auto &getData() const noexcept
        {
            return data;
        }
    };
} // namespace gui
