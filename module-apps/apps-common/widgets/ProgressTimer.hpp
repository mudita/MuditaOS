// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "TimerWithCallbacks.hpp"
#include <Timers/TimerHandle.hpp>
#include <time/time_conversion.hpp>
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
        const std::string name;

      protected:
        std::atomic_bool isRunning{false};
        std::chrono::seconds duration{};
        std::chrono::milliseconds elapsed{};
        std::chrono::seconds interval{};
        std::chrono::milliseconds baseTickInterval{};
        bool hasInterval = false;

        sys::TimerHandle timerTask;

        std::function<void()> onFinishedCallback = nullptr;
        std::function<void()> onIntervalCallback = nullptr;
        ProgressCountdownMode countdownMode;
        utils::time::Duration::DisplayedFormat displayFormat;

        void startTimer();

        [[nodiscard]] auto onTimerTimeout(sys::Timer &timerTask) -> bool;
        [[nodiscard]] auto isFinished() const noexcept -> bool;
        [[nodiscard]] auto intervalReached() const noexcept -> bool;

        virtual void update();

      public:
        ProgressTimer(
            app::ApplicationCommon *app,
            gui::Item &parent,
            std::string timerName,
            std::chrono::milliseconds baseTick,
            ProgressCountdownMode countdownMode                    = ProgressCountdownMode::Decreasing,
            utils::time::Duration::DisplayedFormat displayedFormat = utils::time::Duration::DisplayedFormat::AutoM);
        void reset(std::chrono::seconds _duration,
                   std::chrono::seconds _interval = std::chrono::seconds::zero()) override;
        void start() override;
        void stop() override;
        std::chrono::milliseconds getElapsed() override;
        void registerOnFinishedCallback(std::function<void()> cb) override;
        void registerOnIntervalCallback(std::function<void()> cb) override;
        [[nodiscard]] auto isStopped() const noexcept -> bool override;
    };

} // namespace app
