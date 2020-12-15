// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawFrame.hpp"

namespace gui
{
    DrawFrame::DrawFrame(cpp_freertos::MutexStandard &processingLock, sgui::DrawData &&data)
        : processingLock(processingLock), data(std::move(data))
    {}

    auto DrawFrame::processFrame(FrameProcessor processor, gui::FrameBufferFlyweight &&fb)
        -> std::optional<service::eink::ImageData>
    {
        if (processor == nullptr) {
            return {};
        }

        auto lock = Trylock(processingLock, 10);
        if (not lock.isLocked()) {
            return {};
        }

        state = State::Processed;

        return processor(data, std::move(fb));
    }

} // namespace gui
