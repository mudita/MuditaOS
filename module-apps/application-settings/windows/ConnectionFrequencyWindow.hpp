// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class ConnectionSettings;
}; // namespace app::settingsInterface

namespace gui
{

    namespace window
    {
        inline constexpr auto connection_frequency_window = "ConnectionFrequency";
    };

    class ConnectionFrequencyWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<gui::Option> override;
        void rebuild() override;
        app::settingsInterface::ConnectionSettings *connectionSettings;
        void updateInterval(uint8_t value);

      public:
        ConnectionFrequencyWindow(app::Application *app,
                                  app::settingsInterface::ConnectionSettings *connectionSettings);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };
} // namespace gui
