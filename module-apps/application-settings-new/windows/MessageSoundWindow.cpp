// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageSoundWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    MessageSoundWindow::MessageSoundWindow(app::Application *app) : BaseSettingsWindow(app, window::name::message_sound)
    {
        setTitle(utils::localize.get("app_settings_message_sound"));
    }
} // namespace gui
