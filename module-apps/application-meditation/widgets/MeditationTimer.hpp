// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <atomic>
#include <chrono>

#include <application-meditation/ApplicationMeditation.hpp>

#include "BoxLayout.hpp"
#include "ProgressBar.hpp"
#include "Text.hpp"
#include "Timer.hpp"

namespace gui
{
    class MeditationTimer : public HBox
    {
      public:
        MeditationTimer(std::uint32_t x,
                        std::uint32_t y,
                        std::uint32_t width,
                        std::uint32_t height,
                        app::ApplicationMeditation *app,
                        Item *_parent = nullptr);

        void start();
        void stop();
        void reset(std::chrono::seconds _duration, std::chrono::seconds period = std::chrono::seconds::zero()) noexcept;
        [[nodiscard]] auto isStopped() const noexcept -> bool;
        void onInterval() const;
        void setCounterVisible(bool isVisible) noexcept;
        void registerTimeoutCallback(const std::function<void()> &);

      private:
        void build();
        void update();
        void updateTimer();
        void onReset();

        void startTimer();
        [[nodiscard]] auto onTimerTimeout(Item &self, Timer &timerTask) -> bool;
        [[nodiscard]] auto isFinished() const noexcept -> bool;
        [[nodiscard]] auto intervalReached() const noexcept -> bool;
        [[nodiscard]] auto calculatePercentageValue() const noexcept -> unsigned int;

        std::atomic_bool isRunning{false};
        std::chrono::seconds duration{std::chrono::seconds::zero()};
        std::chrono::seconds elapsed{std::chrono::seconds::zero()};
        std::chrono::seconds intervalPeriod{std::chrono::seconds::zero()};
        bool hasInterval = false;

        app::ApplicationMeditation *application = nullptr;
        CircularProgressBar *progressBar        = nullptr;
        Text *timer                             = nullptr;

        std::function<void()> timeoutCallback = nullptr;
    };
} // namespace gui
