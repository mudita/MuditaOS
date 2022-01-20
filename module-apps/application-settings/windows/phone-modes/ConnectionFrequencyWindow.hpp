// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class ConnectionSettings;
}; // namespace app::settingsInterface

namespace gui
{
    class ConnectionFrequencyWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<gui::Option> override;
        void rebuild() override;
        app::settingsInterface::ConnectionSettings *connectionSettings;
        void updateInterval(uint8_t value);
        std::vector<uint8_t> frequency{0, 15, 30, 45, 60};
        OptionWindowDestroyer destroyer = OptionWindowDestroyer(*this);

      public:
        ConnectionFrequencyWindow(app::ApplicationCommon *app,
                                  app::settingsInterface::ConnectionSettings *connectionSettings);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };
} // namespace gui
