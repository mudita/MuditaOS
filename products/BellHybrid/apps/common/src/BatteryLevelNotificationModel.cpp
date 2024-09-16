// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/BatteryLevelNotificationModel.hpp"

namespace app
{
    BatteryLevelNotificationModel::BatteryLevelNotificationModel()
    {}

    void BatteryLevelNotificationModel::updateBatteryLevelInterval(units::SOC currentBatteryLevel)
    {
        auto updateBatteryIntervalState = [&](BatteryLevelInterval newBatteryLevelIntervalState) {
            if (currentBatteryLevelInterval == newBatteryLevelIntervalState) {
                return;
            }
            currentBatteryLevelInterval     = newBatteryLevelIntervalState;
            isCurrentBatteryIntervalHandled = (newBatteryLevelIntervalState == BatteryLevelInterval::Above15Percent);
        };

        auto updateBatteryIntervalStateWithHysteresis = [&](units::SOC above15PercentHysteresis,
                                                            units::SOC below15PercentHysteresis,
                                                            units::SOC below10PercentHysteresis) {
            if (currentBatteryLevel <= below5PercentMaxVal) {
                updateBatteryIntervalState(BatteryLevelInterval::Below5Percent);
            }
            else if (currentBatteryLevel <= below10PercentMaxVal + below10PercentHysteresis) {
                updateBatteryIntervalState(BatteryLevelInterval::Below10Percent);
            }
            else if (currentBatteryLevel <= below15PercentMaxVal + below15PercentHysteresis) {
                updateBatteryIntervalState(BatteryLevelInterval::Below15Percent);
            }
            else if (currentBatteryLevel >= above15PercentMinVal + above15PercentHysteresis) {
                updateBatteryIntervalState(BatteryLevelInterval::Above15Percent);
            }
        };

        switch (currentBatteryLevelInterval) {
        case (BatteryLevelInterval::Unknown):
        case (BatteryLevelInterval::Above15Percent):
            updateBatteryIntervalStateWithHysteresis(0, 0, 0);
            break;
        case (BatteryLevelInterval::Below15Percent):
            updateBatteryIntervalStateWithHysteresis(batteryLevelIntervalUpperHysteresis, 0, 0);
            break;
        case (BatteryLevelInterval::Below10Percent):
            updateBatteryIntervalStateWithHysteresis(0, batteryLevelIntervalUpperHysteresis, 0);
            break;
        case (BatteryLevelInterval::Below5Percent):
            updateBatteryIntervalStateWithHysteresis(0, 0, batteryLevelIntervalUpperHysteresis);
            break;
        default:
            break;
        }
    }

    BatteryLevelInterval BatteryLevelNotificationModel::getBatteryLevelInterval() const
    {
        return currentBatteryLevelInterval;
    }

    bool BatteryLevelNotificationModel::isBatteryIntervalHandled() const
    {
        return isCurrentBatteryIntervalHandled;
    }

    void BatteryLevelNotificationModel::setCurrentBatteryIntervalAsHandled()
    {
        isCurrentBatteryIntervalHandled = true;
    }

} // namespace app
