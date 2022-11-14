// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationVolumePresenter.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>
#include <common/models/AbstractAudioModel.hpp>

namespace gui
{
    class BellBaseLayout;
    class RelaxationVolumeWindow : public WindowWithTimer
    {
        std::unique_ptr<app::relaxation::AbstractRelaxationVolumePresenter> presenter;
        BellBaseLayout *body{};
        U8IntegerSpinner *spinner = nullptr;

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      public:
        RelaxationVolumeWindow(app::ApplicationCommon *app,
                               std::unique_ptr<app::relaxation::AbstractRelaxationVolumePresenter> &&windowPresenter);
    };
} // namespace gui
