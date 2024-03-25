// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioErrorPresenter.hpp"

#include <Application.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    AudioErrorPresenter::AudioErrorPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void AudioErrorPresenter::activate()
    {
        app::manager::Controller::sendAction(
            app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app->GetName()));
    }
} // namespace gui
