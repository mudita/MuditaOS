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

        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;

        void start();
        void stop();
        void reset(std::chrono::seconds _duration) noexcept;
        auto running() const noexcept -> bool;
        void setTimerVisible(bool) const noexcept;
        void registerTimeoutCallback(const std::function<void()> &);

      private:
        void build();
        void update();
        void updateTimer();
        void onReset();

        void startTimer();
        auto onTimerTimeout(Item &self, Timer &timerTask) -> bool;
        auto isFinished() const noexcept -> bool;
        auto calculatePercentageValue() const noexcept -> unsigned int;

        std::atomic_bool isRunning{false};
        std::chrono::seconds duration{std::chrono::seconds::zero()};
        std::chrono::seconds elapsed{std::chrono::seconds::zero()};

        app::ApplicationMeditation *application = nullptr;
        CircularProgressBar *progressBar        = nullptr;
        Text *timer                             = nullptr;

        std::function<void()> timeoutCallback = nullptr;
    };
} // namespace gui
