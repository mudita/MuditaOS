// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationBasicsPresenter.hpp"
#include <Application.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

namespace app::meditation
{
    MeditationBasicsPresenter::MeditationBasicsPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm)
        : app{app}, alarmModel{alarm}
    {}

    bool MeditationBasicsPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUpFrontLight();
            return true;
        }
        return false;
    }
} // namespace app::meditation
