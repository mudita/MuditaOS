// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageSoundWindow.hpp"

#include "ApplicationSettings.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    MessageSoundWindow::MessageSoundWindow(app::Application *app) : BaseSettingsWindow(app, window::name::message_sound)
    {
        setTitle(utils::translate("app_settings_message_sound"));
    }
} // namespace gui
