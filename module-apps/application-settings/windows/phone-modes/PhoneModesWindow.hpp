// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

namespace app::settingsInterface
{
    class PhoneModeSettings;
}; // namespace app::settingsInterface

namespace gui
{
    class PhoneModesWindow : public OptionWindow
    {
      private:
        auto modesOptList() -> std::list<gui::Option>;
        void onBeforeShow(ShowMode m, SwitchData *d) override;
        app::settingsInterface::PhoneModeSettings *phoneModeSettings;

        const std::vector<std::string> initialModesStrings{utils::translate("app_settings_connected"),
                                                           utils::translate("app_settings_title_do_not_disturb"),
                                                           utils::translate("app_settings_title_offline")};
        std::vector<std::string> modifiedModesStrings{initialModesStrings};

      public:
        PhoneModesWindow(app::Application *app, app::settingsInterface::PhoneModeSettings *phoneModeSettings);
    };
} // namespace gui
