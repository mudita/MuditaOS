// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsVolumePresenter.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

#include <module-audio/Audio/AudioCommon.hpp>
#include <module-audio/Audio/Profiles/Profile.hpp>
namespace gui
{
    class BellBaseLayout;
    class BGSoundsVolumeWindow : public WindowWithTimer, public app::bgSounds::BGSoundsVolumeContract::View
    {
        std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> presenter;
        audio::Volume volume = 0;
        audio::Context audioContext;

        BellBaseLayout *body{};
        UIntegerSpinner *spinner = nullptr;

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data);

      public:
        BGSoundsVolumeWindow(app::ApplicationCommon *app,
                             std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> &&windowPresenter);
    };
} // namespace gui
