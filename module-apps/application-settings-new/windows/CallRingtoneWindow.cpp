// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallRingtoneWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    CallRingtoneWindow::CallRingtoneWindow(app::Application *app) : BaseSettingsWindow(app, window::name::call_ringtone)
    {
        setTitle(utils::localize.get("app_settings_call_ringtome"));
    }
} // namespace gui
