// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// #include "ApplicationBellSettings.hpp"
#include "presenter/BedtimeSettingsPresenter.hpp"
#include <common/widgets/ListViewWithLabels.hpp>
#include <common/options/BellOptionWindow.hpp>

namespace gui
{
    class BellSettingsBedtimeToneWindow : public AppWindow,
                                          public app::bell_settings::BedtimeSettingsWindowContract::View
    {
      public:
        explicit BellSettingsBedtimeToneWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::BedtimeSettingsWindowContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::bell_settings::BedtimeSettingsWindowContract::Presenter> presenter;

        gui::ListViewWithLabels *songList{nullptr};

        void buildInterface() override;
        void handleError() override;
        void rebuild() override;
        void updateViewState() override;
        void activate(const db::multimedia_files::MultimediaFilesRecord &selectedSound);
    };
} /* namespace gui */
