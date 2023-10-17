// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <application-settings/presenter/network/SimPINSettingsPresenter.hpp>

namespace gui
{
    class SimPINSettingsWindow : public BaseSettingsWindow, public SimPINSettingsWindowContract::View
    {
      public:
        explicit SimPINSettingsWindow(app::ApplicationCommon *app,
                                      std::unique_ptr<SimPINSettingsWindowContract::Presenter> simPINPresenter);

      private:
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setNavbarCenterActive(bool state) override;
        void setTitle(const UTF8 &text) override;
        void refreshOptionsList() override;

        std::unique_ptr<SimPINSettingsWindowContract::Presenter> presenter;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
