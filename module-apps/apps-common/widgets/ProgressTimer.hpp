// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "TimerWithCallbacks.hpp"
#include <Timers/TimerHandle.hpp>
#include <atomic>
#include <chrono>
#include <string>
namespace
{
    constexpr auto baseTickDefault = std::chrono::milliseconds{1000};
} // namespace
namespace gui
{
    class Item;
    class Text;
    class Progress;
} // namespace gui

namespace app
{
    class ApplicationCommon;

    enum class ProgressCountdownMode
    {
        Decreasing,
        Increasing
    };
    /** ProgressTimer connects Timer's features with UI representation.
     * The Timer's features consists of:
     * 1) counting time down,
     * 2) ability to perform action (via onIntervalCallback) periodically on reaching an interval
     * 3) ability to perform action (via onFinishedCallback) on reaching countdown end.
     * The UI representation consist of:
     * 1) ability to present time left on attached Text
     * 2) ability to present timer's progress on attached class realising Progress interface.
     */
    class ProgressTimer : public app::TimerWithCallbacks
    {
        app::ApplicationCommon *app = nullptr;
        gui::Item &parent;
        gui::Text *text         = nullptr;
        gui::Progress *progress = nullptr;
        const std::string name;

        std::atomic_bool isRunning{false};
        std::chrono::seconds duration{std::chrono::seconds::zero()};
        std::chrono::seconds elapsed{std::chrono::seconds::zero()};
        std::chrono::seconds interval{std::chrono::seconds::zero()};
        std::chrono::milliseconds baseTickInterval{std::chrono::milliseconds::zero()};
        bool hasInterval = false;

        sys::TimerHandle timerTask;

        std::function<void()> onFinishedCallback = nullptr;
        std::function<void()> onIntervalCallback = nullptr;
        std::function<void()> onBaseTickCallback = nullptr;
        ProgressCountdownMode countdownMode;

        void startTimer();
        void update();
        void updateText();
        void updateProgress();
        void resetProgress();
        [[nodiscard]] auto onTimerTimeout(sys::Timer &timerTask) -> bool;
        [[nodiscard]] auto isFinished() const noexcept -> bool;
        [[nodiscard]] auto intervalReached() const noexcept -> bool;

      public:
        ProgressTimer(app::ApplicationCommon *app,
                      gui::Item &parent,
                      std::string timerName,
                      std::chrono::milliseconds baseTick,
                      ProgressCountdownMode countdownMode = ProgressCountdownMode::Decreasing);
        void reset(std::chrono::seconds _duration,
                   std::chrono::seconds _interval = std::chrono::seconds::zero(),
                   std::chrono::seconds elapsed   = std::chrono::seconds::zero()) override;
        void start() override;
        void stop() override;
        void registerOnFinishedCallback(std::function<void()> cb) override;
        void registerOnIntervalCallback(std::function<void()> cb) override;
        void registerOnBaseTickCallback(std::function<void()> cb) override;
        [[nodiscard]] auto isStopped() const noexcept -> bool override;

        void attach(gui::Progress *_progress);
        void attach(gui::Text *_text);
    };

} // namespace app
