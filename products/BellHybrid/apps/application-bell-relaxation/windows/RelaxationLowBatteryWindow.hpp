// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationLowBatteryPresenter.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class RelaxationLowBatteryWindow : public WindowWithTimer,
                                       public app::relaxation::RelaxationLowBatteryContract::View
    {
      public:
        explicit RelaxationLowBatteryWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::relaxation::RelaxationLowBatteryContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::relaxation::RelaxationLowBatteryContract::Presenter> presenter;
        gui::TextFixedSize *batteryText = nullptr;

        void buildInterface() override;
        bool handleSwitchData(SwitchData *data) override;

        void buildLayout();
        void registerCallbacks();
    };
} // namespace gui
