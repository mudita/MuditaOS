// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellApplicationManager.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>

namespace app::manager
{
    auto BellApplicationManager::startApplication(ApplicationHandle &app) -> bool
    {
        if(not ApplicationManager::startApplication(app))
        {
            LOG_INFO("Starting application %s", app.name().c_str());
            app.run(sys::phone_modes::PhoneMode::Offline, this);
        }
        return true;
    }

    auto BellApplicationManager::resolveHomeApplication() -> std::string
    {
        if (checkOnBoarding()) {
            return app::name_onboarding;
        }
        return rootApplicationName;
    }
} // namespace app::manager
