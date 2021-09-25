// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsMainWindowPresenter.hpp"
#include <common/options/BellOptionWindow.hpp>
namespace gui
{
    class BGSoundsMainWindow : public BellOptionWindow, public app::bgSounds::BGSoundsMainWindowContract::View
    {
        std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> presenter;

        void setSoundsList(std::vector<tags::fetcher::Tags> soundsTags);
        void buildInterface() override;

        void onActivated(const tags::fetcher::Tags &selectedSoundTags);

      public:
        BGSoundsMainWindow(app::ApplicationCommon *app,
                           std::unique_ptr<app::bgSounds::BGSoundsMainWindowContract::Presenter> &&presenter);
    };
} // namespace gui
