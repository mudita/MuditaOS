// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "ReadyGoingPresenter.hpp"

namespace app::meditation
{
    ReadyGoingPresenter::ReadyGoingPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void ReadyGoingPresenter::activate()
    {
        app->switchWindow(gui::name::window::meditationProgress);
    }
} // namespace app::meditation
