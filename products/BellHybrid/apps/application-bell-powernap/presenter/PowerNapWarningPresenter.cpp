// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapWarningPresenter.hpp"
#include <ApplicationBellPowerNap.hpp>

namespace app::powernap
{
    PowerNapWarningPresenter::PowerNapWarningPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarm)
        : app{app}, alarmModel{alarm}
    {}

    void PowerNapWarningPresenter::activate()
    {
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;
        app->switchWindow(gui::window::name::powerNapProgress, std::move(data));
    }
    bool PowerNapWarningPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUp();
            return true;
        }
        return false;
    }
} // namespace app::powernap
