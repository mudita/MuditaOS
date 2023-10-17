// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "ReadyGoingPresenter.hpp"
#include "models/StartDelay.hpp"

namespace app::meditation
{
    ReadyGoingPresenter::ReadyGoingPresenter(app::ApplicationCommon *app, const models::StartDelay &startDelayModel)
        : app{app}, startDelayModel{startDelayModel}
    {}

    void ReadyGoingPresenter::activate()
    {
        app->switchWindow(windows::meditationProgress);
    }
    std::chrono::seconds ReadyGoingPresenter::getStartDelay()
    {
        return std::chrono::seconds{startDelayModel.getValue()};
    }
} // namespace app::meditation
