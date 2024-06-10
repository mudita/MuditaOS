// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewPresenter.hpp"

namespace app::whatsNew
{
    WhatsNewPresenter::WhatsNewPresenter(AbstractBatteryModel &batteryModel,
                                         AbstractLowBatteryInfoModel &lowBatteryInfoModel)
        : batteryModel{batteryModel}, lowBatteryInfoModel{lowBatteryInfoModel}
    {}

    Store::Battery WhatsNewPresenter::getBatteryState()
    {
        return batteryModel.getLevelState();
    }

    bool WhatsNewPresenter::isBatteryCharging(Store::Battery::State state) const
    {
        return batteryModel.isBatteryCharging(state);
    }

    bool WhatsNewPresenter::isBatteryBelowLowLevelThreshold(units::SOC soc) const
    {
        return soc < constants::lowBatteryInfoThreshold;
    }

    bool WhatsNewPresenter::isLowBatteryWindowHandled() const
    {
        return lowBatteryInfoModel.isInfoHandled();
    }

    void WhatsNewPresenter::handleLowBatteryWindow()
    {
        lowBatteryInfoModel.handleInfo();
    }
} // namespace app::whatsNew
