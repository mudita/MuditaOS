// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapSessionPausedPresenter.hpp"
#include <Application.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace app::powernap
{
    PowerNapSessionPausedPresenter::PowerNapSessionPausedPresenter(app::ApplicationCommon *app,
                                                                   AbstractAlarmModel &alarm)
        : app{app}, alarmModel{alarm}
    {}

    bool PowerNapSessionPausedPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUpFrontLight();
            return true;
        }
        return false;
    }
} // namespace app::powernap
