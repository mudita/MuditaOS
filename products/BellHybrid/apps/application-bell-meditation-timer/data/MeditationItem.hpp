// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace meditation
{
    namespace value
    {
        inline constexpr auto TIMER_DEF = 20 * 60;
        inline constexpr auto TIMER_MAX = 180 * 60;
        inline constexpr auto TIMER_MIN = 1 * 60;
    } // namespace value

    class MeditationItem
    {
      private:
        uint32_t timerSecs;
        uint32_t intervalSecs;
        uint32_t passedSecs;

      public:
        explicit MeditationItem() : timerSecs{value::TIMER_DEF}, intervalSecs{0}, passedSecs{0}
        {}
        virtual ~MeditationItem() = default;

        inline auto getTimerSecs()
        {
            return timerSecs;
        }

        inline void setTimerSecs(uint32_t secs)
        {
            timerSecs = secs;
        }

        inline auto getIntervalSecs()
        {
            return intervalSecs;
        }

        inline void setIntervalSecs(uint32_t secs)
        {
            intervalSecs = secs;
        }

        inline auto getPassedSecs()
        {
            return passedSecs;
        }

        inline void setPassedSecs(uint32_t secs)
        {
            passedSecs = secs;
        }
    };

} // namespace meditation
