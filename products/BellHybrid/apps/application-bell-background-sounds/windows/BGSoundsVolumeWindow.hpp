// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsVolumePresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class SideListView;
    class BGSoundsVolumeWindow : public WindowWithTimer, public app::bgSounds::BGSoundsVolumeContract::View
    {
        std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> presenter;
        SideListView *sideListView = nullptr;

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      public:
        BGSoundsVolumeWindow(app::ApplicationCommon *app,
                             std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> &&windowPresenter);
    };
} // namespace gui
