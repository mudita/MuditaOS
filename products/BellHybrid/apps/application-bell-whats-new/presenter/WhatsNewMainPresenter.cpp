// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewMainPresenter.hpp"
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <product/version.hpp>

namespace app::whatsnew
{
    WhatsNewMainPresenter::WhatsNewMainPresenter(settings::Settings *settings) : settings{settings}
    {}

    auto WhatsNewMainPresenter::setCurrentOsVersion() -> void
    {
        settings->setValue(settings::SystemProperties::osCurrentVersion, VERSION, settings::SettingsScope::Global);
    }
} // namespace app::whatsnew
