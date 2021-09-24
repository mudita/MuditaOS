// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsMainWindowPresenter.hpp"
#include <AppWindow.hpp>
namespace gui
{
    class BGSoundsMainWindow : public AppWindow, public app::bgSounds::BGSoundsMainWindowContract::View
    {
        std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> presenter;

        auto onInput(const InputEvent &inputEvent) -> bool override;

      public:
        BGSoundsMainWindow(app::ApplicationCommon *app,
                           std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> &&presenter);
    };
} // namespace gui
