// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class KeypadLightWindow : public BaseSettingsWindow
    {
      public:
        KeypadLightWindow(app::ApplicationCommon *app, app::settingsInterface::KeypdBacklightSettings *settings);

      private:
        app::settingsInterface::KeypdBacklightSettings *keypadLightSettings;
        void switchHandler(bool &onOffSwitch);
        std::list<Option> buildOptionsList() override;

        bool isActiveSwitchOn               = false;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
