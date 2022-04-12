// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace bluetooth
{

    class BatteryLevel
    {
        static constexpr auto batteryLowValue = 0;
        static constexpr auto batteryMaxValue = 100;

      public:
        explicit BatteryLevel(unsigned int level) : level(level){};
        BatteryLevel() = default;
        auto getBatteryLevel() const -> unsigned int
        {
            return std::clamp<unsigned int>(level, batteryLowValue, batteryMaxValue);
        }
        auto getBatteryLevelBars() const -> unsigned int
        {
            constexpr auto level1Threshold = 15;
            constexpr auto level2Threshold = 35;
            constexpr auto level3Threshold = 55;
            constexpr auto level4Threshold = 75;
            constexpr auto level5Threshold = 95;

            if (level < level1Threshold) {
                return 0;
            }
            else if (level < level2Threshold) {
                return 1;
            }
            else if (level < level3Threshold) {
                return 2;
            }
            else if (level < level4Threshold) {
                return 3;
            }
            else if (level < level5Threshold) {
                return 4;
            }
            else {
                return 5;
            }
        }

      private:
        unsigned int level = 0;
    };
} // namespace bluetooth
