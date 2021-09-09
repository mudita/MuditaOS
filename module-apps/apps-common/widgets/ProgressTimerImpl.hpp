// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include "ProgressTimer.hpp"
#include <apps-common/GuiTimer.hpp>
#include <chrono>
#include <atomic>
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

    class ProgressTimerImpl : public ProgressTimer
    {
        app::ApplicationCommon *app = nullptr;
        gui::Item *parent       = nullptr;
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

        void startTimer();
        void update();
        void updateText();
        void updateProgress();
        void resetProgress();
        [[nodiscard]] auto onTimerTimeout(gui::Item &self, sys::Timer &timerTask) -> bool;
        [[nodiscard]] auto isFinished() const noexcept -> bool;
        [[nodiscard]] auto intervalReached() const noexcept -> bool;

      public:
        ProgressTimerImpl(app::ApplicationCommon *app,
                          gui::Item *parent,
                          std::string timerName,
                          std::chrono::milliseconds baseTick);
        void reset(std::chrono::seconds _duration,
                   std::chrono::seconds _interval = std::chrono::seconds::zero()) override;
        void start() override;
        void stop() override;
        void registerOnFinishedCallback(std::function<void()> cb) override;
        void registerOnIntervalCallback(std::function<void()> cb) override;
        [[nodiscard]] auto isStopped() const noexcept -> bool override;

        void attach(gui::Progress *_progress) override;
        void attach(gui::Text *_text) override;
    };

} // namespace app
