// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/network/ApnSettingsModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{

    class ApnOptionsWindow : public BaseSettingsWindow
    {
      public:
        ApnOptionsWindow(app::ApplicationCommon *app);

      private:
        auto buildOptionsList() -> std::list<gui::Option> override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        std::shared_ptr<packet_data::APN::Config> apn;
        ApnSettingsModel apnSettingsModel;
    };
} // namespace gui
