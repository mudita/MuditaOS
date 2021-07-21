// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/presenter/system/TechnicalWindowPresenter.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class TechnicalInformationWindow : public AppWindow, public TechnicalWindowContract::View
    {

      public:
        TechnicalInformationWindow(app::Application *app,
                                   std::shared_ptr<TechnicalWindowContract::Presenter> presenter);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void imeiReady() noexcept override;

        ListView *list = nullptr;
        std::shared_ptr<TechnicalWindowContract::Presenter> presenter;
    };

} // namespace gui
