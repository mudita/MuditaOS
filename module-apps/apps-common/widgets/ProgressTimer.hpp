// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <functional>

namespace gui
{
    class Text;
    class Progress;
} // namespace gui

namespace app
{
    class ProgressTimerUIConfigurator
    {

      public:
        virtual ~ProgressTimerUIConfigurator()       = default;
        virtual void attach(gui::Progress *progress) = 0;
        virtual void attach(gui::Text *clock)        = 0;
    };

    /** ProgressTimer provides an interface that connect Timer's features to UI representation.
     * The Timer's features consists of:
     * 1) counting time down,
     * 2) ability to perform action (via onIntervalCallback) periodically on reaching an interval
     * 3) ability to perform action (via onFinishedCallback) on reaching countdown end.
     * The UI representation consist of:
     * 1) ability to present time left on attached Text
     * 2) ability to present timer's progress on attached class realising Progress interface.
     */
    class ProgressTimer : public ProgressTimerUIConfigurator
    {
      public:
        [[nodiscard]] virtual auto isStopped() const noexcept -> bool                    = 0;
        virtual void reset(std::chrono::seconds duration,
                           std::chrono::seconds interval = std::chrono::seconds::zero(),
                           std::chrono::seconds elapsed  = std::chrono::seconds::zero())  = 0;
        virtual void start()                                                             = 0;
        virtual void stop()                                                              = 0;
        virtual void registerOnFinishedCallback(std::function<void()> cb)                = 0;
        virtual void registerOnIntervalCallback(std::function<void()> cb)                = 0;
        virtual void registerOnBaseTickCallback(std::function<void()> cb)                = 0;

        virtual void attach(gui::Progress *progress) = 0;
        virtual void attach(gui::Text *text)         = 0;
    };

} // namespace app
