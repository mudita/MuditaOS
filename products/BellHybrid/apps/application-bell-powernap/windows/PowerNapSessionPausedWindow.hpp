// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/apps-common/BasePresenter.hpp"
#include <common/windows/SessionPausedWindow.hpp>
#include "presenter/PowerNapSessionPausedPresenter.hpp"

namespace gui
{
    class PowerNapSessionPausedWindow : public SessionPausedWindow,
                                        public app::powernap::PowerNapSessionPausedContract::View
    {
      public:
        explicit PowerNapSessionPausedWindow(
            app::ApplicationCommon *app,
            std::shared_ptr<app::powernap::PowerNapSessionPausedContract::Presenter> presenter);

        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::shared_ptr<app::powernap::PowerNapSessionPausedContract::Presenter> presenter;
    };
} // namespace gui
