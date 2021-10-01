// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>

namespace app::home_screen
{
    BatteryModel::BatteryModel(app::ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    Store::Battery BatteryModel::getLevelState() const
    {
        return Store::Battery::get();
    }
} // namespace app::home_screen
