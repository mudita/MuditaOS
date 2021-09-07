// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/network/ApnSettingsModel.hpp>

#include <Icon.hpp>
#include <OptionWindow.hpp>

namespace gui
{
    class ApnSettingsWindow : public OptionWindow
    {
      public:
        ApnSettingsWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        auto optionsList(std::vector<std::shared_ptr<packet_data::APN::Config>> vector) -> std::list<Option>;

        Icon *emptyListIcon = nullptr;
        std::vector<std::shared_ptr<packet_data::APN::Config>> apns;
        std::shared_ptr<packet_data::APN::Config> activeApn;
        std::shared_ptr<ApnSettingsModel> apnSettingsModel;
    };
}; // namespace gui
