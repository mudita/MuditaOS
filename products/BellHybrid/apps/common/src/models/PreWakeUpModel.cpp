// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/PreWakeUpModel.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <service-time/AlarmServiceAPI.hpp>

namespace app
{
    PreWakeUpModel::PreWakeUpModel(ApplicationCommon *app) : app{app}
    {}

    bool PreWakeUpModel::isActive() const
    {
        return activity;
    }

    void PreWakeUpModel::setActive(bool active)
    {
        activity = active;
    }

    bool PreWakeUpModel::turnOffPreWakeUp()
    {
        return alarms::AlarmServiceAPI::requestTurnOffPreWakeUp(app);
    }
} // namespace app
