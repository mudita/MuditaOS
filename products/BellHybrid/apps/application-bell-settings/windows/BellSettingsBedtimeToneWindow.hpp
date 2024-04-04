// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/BedtimeSettingsPresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsBedtimeToneWindow : public AppWindow,
                                          public app::bell_settings::BedtimeSettingsWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsBedtimeTone";
        explicit BellSettingsBedtimeToneWindow(
            app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::SettingsPresenter::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;
        void deepRefresh() override;
        void handleError() override;
        void handleDeletedFile() override;

      private:
        SideListView *sidelistview{};
        std::unique_ptr<app::bell_settings::SettingsPresenter::Presenter> presenter;
    };
} /* namespace gui */
