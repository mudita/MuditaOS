// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/core/Context.hpp>
#include <memory>
#include <queue>
#include <gui/core/DrawCommandForward.hpp>
#include <service-eink/ImageData.hpp>
#include <utility>
#include "DrawData.hpp"
#include <functional>
#include "TryLock.hpp"

namespace gui
{
    class DrawCommand;

    class FrameBuffer
    {
      public:
        Size size;
        Context canvas;

        FrameBuffer()                    = delete;
        FrameBuffer(const FrameBuffer &) = delete;
        FrameBuffer(FrameBuffer &&)      = delete;
        FrameBuffer &operator=(const FrameBuffer &) = delete;
        FrameBuffer &operator=(FrameBuffer &&) = delete;
        virtual ~FrameBuffer()                 = default;

        FrameBuffer(Size size) : size(size), canvas(size)
        {}
    };
} // namespace gui

namespace service::renderer
{

    using FrameProcessor = std::function<bool(sgui::DrawData &data, gui::FrameBuffer &fb)>;

    class DrawFrame
    {
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
        DrawFrame(cpp_freertos::MutexStandard &processingLock, sgui::DrawData &&data, gui::Size canvasSize)
            : processingLock(processingLock), data(std::move(data))
        {}

        bool processFrame(FrameProcessor processor, gui::FrameBuffer &fb)
        {
            if (processor == nullptr) {
                return false;
            }

            auto lock = Trylock(processingLock, 10);
            if (not lock.isLocked()) {
                return false;
            }

            state = State::Processed;
            return processor(data, fb);
        }

        [[nodiscard]] auto getState() const noexcept
        {
            return state;
        }

        auto &getData()
        {
            return data;
        }
    };

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
            this->drawFrames.emplace_back(DrawFrame{lockProcessing, std::move(data), size});
            return true;
        }

        /// TODO WIP
        bool processLastFrame(FrameProcessor processor, gui::FrameBuffer &fb)
        {
            auto lock = Trylock(lockQueue, 10);
            if (not lock.isLocked()) {
                return false;
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
                return false;
            }

            // TODO lock selection of frame selected to be processed
            // then mark it as "being processed" ?

            return drawFrames.back().processFrame(processor, fb);
        }

        /// take last processed frame to send to eink
        /// lock as we will give frame results to somebody and then remove it from this context
        bool takeLastProcessedFrame(service::eink::ImageData &data)
        {
            auto lock = Trylock(lockQueue, 10);
            if (not lock.isLocked()) {
                return false;
            }

            if (drawFrames.empty()) {
                return false;
            }

            // TODO frameskip if there are more than 1 frame ready
            data.mode    = drawFrames.front().getData().getMode();
            // ok now what... we should use FrameBuffer here?
            // or somehow pass it another way?
            // how should we pass it then?
            data.context = drawFrames.front().takeContext();
            drawFrames.pop_back();

            return true;
        }
    };
} // namespace service::renderer
