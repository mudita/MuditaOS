// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/BatteryModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>

namespace app
{
    BatteryModel::BatteryModel(app::ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    Store::Battery BatteryModel::getLevelState() const
    {
        return Store::Battery::get();
    }

    bool BatteryModel::isBatteryCharging(Store::Battery::State state) const
    {
        return (state == Store::Battery::State::Charging) or (state == Store::Battery::State::ChargingDone);
    }
} // namespace app
