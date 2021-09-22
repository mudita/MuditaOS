// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace meditation
{
    namespace value
    {
        inline constexpr auto defaultTimerValue = std::chrono::seconds{15 * 60};
        inline constexpr auto maxTimerValue     = std::chrono::seconds{180 * 60};
        inline constexpr auto minTimerValue     = std::chrono::seconds{1 * 60};
    } // namespace value

    class MeditationItem
    {
      private:
        std::chrono::seconds timer;
        std::chrono::seconds interval;
        std::chrono::seconds passed;

      public:
        MeditationItem()
            : timer{value::defaultTimerValue}, interval{std::chrono::seconds::zero()}, passed{
                                                                                           std::chrono::seconds::zero()}
        {}

        inline auto getTimer() const noexcept
        {
            return timer;
        }

        inline void setTimer(std::chrono::seconds value) noexcept
        {
            timer = value;
        }

        inline auto getInterval() const noexcept
        {
            return interval;
        }

        inline void setInterval(std::chrono::seconds value) noexcept
        {
            interval = value;
        }

        inline auto getPassed() const noexcept
        {
            return passed;
        }

        inline void setPassed(std::chrono::seconds value) noexcept
        {
            passed = value;
        }
    };

} // namespace meditation
