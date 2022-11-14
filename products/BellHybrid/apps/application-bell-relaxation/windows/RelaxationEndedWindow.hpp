// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationEndedPresenter.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class RelaxationEndedWindow : public WindowWithTimer, public app::relaxation::RelaxationEndedContract::View
    {
      public:
        explicit RelaxationEndedWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::relaxation::RelaxationEndedContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::relaxation::RelaxationEndedContract::Presenter> presenter;

        void buildInterface() override;

        void registerCallbacks();
        void buildLayout();
    };
} // namespace gui
