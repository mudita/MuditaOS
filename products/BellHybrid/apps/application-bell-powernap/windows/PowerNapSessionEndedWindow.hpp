// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/PowerNapSessionEndedPresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class PowerNapSessionEndedWindow : public WindowWithTimer, public app::powernap::PowerNapSessionEndedContract::View
    {
        std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter;
        void buildInterface() override;
        void buildLayout();
        void registerCallbacks();

      public:
        explicit PowerNapSessionEndedWindow(
            app::Application *app, std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter);
    };
} // namespace gui
