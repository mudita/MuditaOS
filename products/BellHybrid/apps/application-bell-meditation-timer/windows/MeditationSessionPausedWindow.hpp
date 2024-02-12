// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/apps-common/BasePresenter.hpp"
#include <common/windows/SessionPausedWindow.hpp>
#include <presenter/MeditationBasicsPresenter.hpp>

namespace gui
{
    class MeditationSessionPausedWindow : public SessionPausedWindow,
                                          public app::meditation::MeditationBasicsContract::View
    {
      public:
        explicit MeditationSessionPausedWindow(
            app::ApplicationCommon *app,
            std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter);

        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter;
    };
} // namespace gui
