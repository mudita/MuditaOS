// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/apps-common/BasePresenter.hpp"
#include <common/windows/BellFinishedWindow.hpp>
#include <presenter/MeditationBasicsPresenter.hpp>

namespace gui
{
    class MeditationFinishWindow : public BellFinishedWindow, public app::meditation::MeditationBasicsContract::View
    {
      public:
        explicit MeditationFinishWindow(
            app::ApplicationCommon *app,
            const std::string &name,
            std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter);

        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter;
    };
} // namespace gui
