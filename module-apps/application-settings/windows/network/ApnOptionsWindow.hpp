// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "network/ApnSettingsModel.hpp"

#include <apps-common/windows/BaseSettingsWindow.hpp>
#include <service-cellular/PacketDataTypes.hpp>

namespace gui
{

    class ApnOptionsWindow : public BaseSettingsWindow
    {
      public:
        ApnOptionsWindow(app::Application *app);

      private:
        auto buildOptionsList() -> std::list<gui::Option> override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        std::shared_ptr<packet_data::APN::Config> apn;
        ApnSettingsModel *apnSettingsModel = nullptr;
    };
} // namespace gui
