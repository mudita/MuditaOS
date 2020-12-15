// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <queue>
#include <gui/core/DrawCommandForward.hpp>
#include <utility>
#include <functional>
#include "FrameBufferPool.hpp"
#include "DrawFrame.hpp"

namespace gui
{
    class DrawCommand;

} // namespace gui

namespace service::renderer
{


    class FrameStack
    {
        gui::Size size;
        cpp_freertos::MutexStandard lockQueue;
        cpp_freertos::MutexStandard lockProcessing;
        std::deque<DrawFrame> drawFrames;

      public:
        FrameStack(::gui::Size size) : size(size)
        {}

        bool emplaceDrawData(sgui::DrawData &&data)
        {
            auto lock = Trylock(lockQueue, 10);
            if (not lock.isLocked()) {
                return false;
            }
            this->drawFrames.emplace_back(DrawFrame{lockProcessing, std::move(data)});
            return true;
        }

        /// TODO WIP - need to process it separatelly - need to process it separatelly
        std::optional<DrawFrame> getLastFrame(gui::FrameBuffer &fb)
        {
            auto lock = Trylock(lockQueue, 10);
            if (not lock.isLocked()) {
                return {};
            }

            // for ( auto & f : drawFrames )
            // {
            //   if( f.back().getData().getMode() == gui::RefreshModes::GUI_REFRESH_DEEP )
            //   {
            //       drawFrames.back().setDeepRefresh();
            //       continue;
            //   }
            // }

            // DROP frames:
            // 1 .. end() -1

            if (drawFrames.empty()) {
                return {};
            }

            // TODO lock selection of frame selected to be processed
            // TODO do I need to remove last element after moving it?
            return std::move(drawFrames.back());
        }
    };
} // namespace service::renderer
