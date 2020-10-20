// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputLanguageWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{

    InputLanguageWindow::InputLanguageWindow(app::Application *app)
        : BaseSettingsWindow(app, window::name::input_language)
    {
        buildInterface();
    }

    void InputLanguageWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_input_language"));
    }
} // namespace gui
