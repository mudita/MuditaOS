// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/PowerNapSessionEndedPresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class PowerNapSessionEndedWindow : public WindowWithTimer, public app::powernap::PowerNapSessionEndedContract::View
    {
        static constexpr auto image_top_margin    = 170U;
        static constexpr auto image_bottom_margin = 30U;
        std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter;
        void buildInterface() override;
        void buildLayout();
        void registerCallbacks();

      public:
        explicit PowerNapSessionEndedWindow(
            app::ApplicationCommon *app,
            std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter);
    };
} // namespace gui
