// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusMainPresenter.hpp"

namespace app::focus
{
    FocusMainPresenter::FocusMainPresenter(AbstractBatteryModel &batteryModel,
                                           AbstractLowBatteryInfoModel &lowBatteryInfoModel)
        : batteryModel{batteryModel}, lowBatteryInfoModel{lowBatteryInfoModel}
    {}

    Store::Battery FocusMainPresenter::getBatteryState()
    {
        return batteryModel.getLevelState();
    }

    bool FocusMainPresenter::isBatteryCharging(Store::Battery::State state) const
    {
        return batteryModel.isBatteryCharging(state);
    }

    bool FocusMainPresenter::isBatteryBelowLowLevelThreshold(units::SOC soc) const
    {
        return soc < constants::lowBatteryInfoThreshold;
    }

    bool FocusMainPresenter::isLowBatteryWindowHandled() const
    {
        return lowBatteryInfoModel.isInfoHandled();
    }

    void FocusMainPresenter::handleLowBatteryWindow()
    {
        lowBatteryInfoModel.handleInfo();
    }
} // namespace app::focus
