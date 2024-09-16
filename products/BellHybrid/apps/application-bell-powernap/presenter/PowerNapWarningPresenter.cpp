// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerNapWarningPresenter.hpp"
#include <ApplicationBellPowerNap.hpp>

namespace app::powernap
{
    PowerNapWarningPresenter::PowerNapWarningPresenter(app::ApplicationCommon *app) : app{app}
    {}

    void PowerNapWarningPresenter::activate()
    {
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;
        app->switchWindow(gui::window::name::powerNapProgress, std::move(data));
    }
} // namespace app::powernap
