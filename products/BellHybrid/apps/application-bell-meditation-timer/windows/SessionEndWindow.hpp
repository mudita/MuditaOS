// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SessionEndedPresenter.hpp"

#include <common/windows/BellFinishedWindow.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class SessionEndWindow : public BellFinishedWindow
    {
      private:
        std::shared_ptr<app::meditation::SessionEndedPresenterContract::Presenter> presenter;

      public:
        SessionEndWindow(app::ApplicationCommon *app,
                         std::shared_ptr<app::meditation::SessionEndedPresenterContract::Presenter> winPresenter);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} // namespace gui
