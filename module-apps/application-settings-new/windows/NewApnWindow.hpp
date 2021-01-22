// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/models/ApnSettingsModel.hpp"
#include "application-settings-new/models/NewApnModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Text.hpp>

namespace gui
{
    class NewApnWindow : public AppWindow
    {
      public:
        NewApnWindow(app::Application *app);

      private:
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto handleSwitchData(SwitchData *data) -> bool override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        auto verifyAndSave() -> bool;
        void setSaveButtonVisible(bool visible);
        std::shared_ptr<packet_data::APN::Config> apn;
        std::shared_ptr<NewApnModel> newApnModel;
        ApnSettingsModel *apnSettingsModel = nullptr;
        gui::ListView *list                = nullptr;
    };

} /* namespace gui */
