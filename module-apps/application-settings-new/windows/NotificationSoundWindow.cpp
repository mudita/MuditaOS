// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationSoundWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    NotificationSoundWindow::NotificationSoundWindow(app::Application *app)
        : BaseSettingsWindow(app, window::name::notification_sound)
    {
        setTitle(utils::localize.get("app_settings_notification_sound"));
    }
} // namespace gui
