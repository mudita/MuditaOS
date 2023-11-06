// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class OperatorsSettings;
}; // namespace app::settingsInterface

namespace gui
{
    class NetworkWindow : public BaseSettingsWindow
    {
      public:
        NetworkWindow(app::ApplicationCommon *app, app::settingsInterface::OperatorsSettings *operatorsSettings);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        auto buildOptionsList() -> std::list<Option> override;
        auto getVoltePrimaryLabel() -> std::string;
        auto getVolteSecondaryLabel() -> std::string;
        auto getRightItemSetting() -> option::SettingRightItem;

        app::settingsInterface::OperatorsSettings *operatorsSettings = nullptr;
        OptionWindowDestroyer rai_destroyer                          = OptionWindowDestroyer(*this);
    };
} // namespace gui
