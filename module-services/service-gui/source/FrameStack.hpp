// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/core/Context.hpp>
#include <memory>
#include <queue>
#include <mutex.hpp>
#include <gui/core/DrawCommandForward.hpp>
#include <service-eink/ImageData.hpp>
#include <utility>
#include "DrawData.hpp"
#include <functional>

namespace gui
{
    class DrawCommand;
}

namespace service::renderer
{

    using FrameProcessor = std::function<bool(sgui::DrawData &data, ::gui::Context &context)>;

    class Trylock
    {
        cpp_freertos::Mutex &mutex;
        bool locked = false;

      public:
        Trylock(cpp_freertos::Mutex &m, uint32_t timeout) : mutex(m)
        {
            locked = mutex.Lock(timeout);
        }

        ~Trylock()
        {
            if (locked) {
                mutex.Unlock();
            }
        }

        [[nodiscard]] auto isLocked() const noexcept
        {
            return locked;
        }
    };

    // TODO Frame {
    //    - drawCommands if avail
    //    - context to draw on
    //    - lock onDrawCommands
    //
    //
    //    do we need separate lock to draw on, or we could just lock FrameStack?
    //    yes & no
    //
    //    we can queue next frame
    //    prev frame doesn't rebuild queue
    //
    //    so we can process on data for context & draw commands
    //
    //    only:
    //    -> adding Frame
    //    <- removing Frame
    //    needs to be locked
    //
    //    ok... so we need to lock processing -_-"
    //
    //    so we cant remove frame we are working on
    //    so we have to await -> so separate lock for this time then?
    //      - frame in processing : gets new lock ( so that we don't need locking stack )
    //      - when frame is processing : it asumes procesing lock
    //      - when it ends - it unlocks
    //      - frame has state:
    //         > fresh > processing > processed
    //      - we have state though ... that we are processing frame : so do we need it?
    //      - or we dont need state?
    //
    //  ...
    //  so all frames have reference to `processing` lock from FrameStack
    //  and frame stack have single lock for adding/removing frames
    // }
    //
    //
    // ok... maybe queue is overengineered...
    // at max we need 3 frames:
    // 1. one being send to eink
    // 2. one being rendered
    // 3. one slot available for swap
    // no need for queue
    // we can at once render only one frame on one canvas
    // so we need:
    // - 2 canvas to draw (one to draw on,      one to process data)
    // - 2 commands slots (one to be processed, one to accept new data)
    // this leaves us with canvas which should potentially have more memory allocated then needed
    //
    // so maybe - do not allocate context as long as it's not needed?

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
        ::gui::Context context;

        State state = State::Fresh;

      public:
        DrawFrame(cpp_freertos::MutexStandard &processingLock, sgui::DrawData &&data, gui::Size canvasSize)
            : processingLock(processingLock), data(std::move(data)), context(canvasSize)
        {}

        /// function to be called in worker processing Frame data
        /// TODO FEED LOCK HERE pass foo with scoped lock guard so that it would be testable
        bool processFrame(FrameProcessor processor)
        {
            if (processor == nullptr) {
                return false;
            }

            auto lock = Trylock(processingLock, 10);
            if (not lock.isLocked()) {
                return false;
            }

            state = State::Processed;
            return processor(data, context);
        }

        [[nodiscard]] auto getState() const noexcept
        {
            return state;
        }

        auto &getData()
        {
            return data;
        }

        ::gui::Context takeContext()
        {
            return std::move(context);
        }
    };

    class FrameStack
    {
        ::gui::Size size;

        // lock for take/give new elements
        cpp_freertos::MutexStandard lockQueue;
        // lock for processing slected frame
        // removing frames / rebuilidng queue needs both locks:
        // 1. lock for processing (to not remoce memory we are working on)
        cpp_freertos::MutexStandard lockProcessing;

        std::deque<DrawFrame> drawFrames;

      public:
        FrameStack(::gui::Size size) : size(size)
        {}

        /// trylock? and then what
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
        bool processLastFrame(FrameProcessor processor)
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

            return drawFrames.back().processFrame(processor);
        }

        /// take last processed frame to send to eink
        /// lock as we will give frame results to somebody and then remove it from this context
        bool takeLastProcessedFrame(service::eink::ImageData &data)
        {
            auto lock = Trylock(lockQueue, 10);
            if (not lock.isLocked()) {
                return false;
            }

            // TODO frameskip if there are more than 1 frame ready
            data.mode    = drawFrames.front().getData().getMode();
            data.context = drawFrames.front().takeContext();
            drawFrames.pop_back();

            return true;
        }
    };
} // namespace service::renderer
