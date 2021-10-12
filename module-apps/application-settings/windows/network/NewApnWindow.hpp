// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/network/ApnSettingsModel.hpp>
#include <application-settings/models/network/NewApnModel.hpp>

#include <ListView.hpp>
#include <Text.hpp>
#include <AppWindow.hpp>

namespace gui
{
    class NewApnWindow : public AppWindow
    {
      public:
        NewApnWindow(app::ApplicationCommon *app);

      private:
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void setSaveButtonVisible(bool visible);
        std::shared_ptr<packet_data::APN::Config> apn;
        std::shared_ptr<NewApnModel> newApnModel;
        ApnSettingsModel *apnSettingsModel = nullptr;
        gui::ListView *list                = nullptr;
    };

} /* namespace gui */
