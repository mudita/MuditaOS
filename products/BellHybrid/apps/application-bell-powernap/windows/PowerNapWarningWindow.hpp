// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "presenter/PowerNapWarningPresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class PowerNapWarningWindow : public WindowWithTimer, public app::powernap::PowerNapWarningContract::View
    {
      public:
        PowerNapWarningWindow(app::ApplicationCommon *app,
                              std::shared_ptr<app::powernap::PowerNapWarningContract::Presenter> presenter);

      protected:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildLayout();
        void registerCallbacks();

      private:
        std::shared_ptr<app::powernap::PowerNapWarningContract::Presenter> presenter;
        Icon *icon{};
    };
} // namespace gui
