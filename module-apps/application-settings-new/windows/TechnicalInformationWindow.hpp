// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include <module-apps/application-settings-new/ApplicationSettings.hpp>
#include <module-apps/application-settings-new/presenter/TechnicalWindowPresenter.hpp>

namespace gui
{
    class TechnicalInformationWindow : public AppWindow, public TechnicalWindowContract::View
    {
        static constexpr auto imei = "AA-BBBBBB-CCCCC-D";

      public:
        TechnicalInformationWindow(app::Application *app,
                                   std::unique_ptr<TechnicalWindowContract::Presenter> presenter);
        ~TechnicalInformationWindow() override;

        void destroyInterface() override;

      private:
        void buildInterface() override;

        gui::Text *modelText  = nullptr;
        gui::Text *modelValue = nullptr;

        gui::Text *serialNumberText  = nullptr;
        gui::Text *serialNumberValue = nullptr;

        gui::Text *osVersionText  = nullptr;
        gui::Text *osVersionValue = nullptr;

        gui::Text *imeiText  = nullptr;
        gui::Text *imeiValue = nullptr;

        gui::Text *batteryText  = nullptr;
        gui::Text *batteryValue = nullptr;

        gui::Text *pcbMbText  = nullptr;
        gui::Text *pcbMbValue = nullptr;

        gui::Text *pcbAmText  = nullptr;
        gui::Text *pcbAmValue = nullptr;

        gui::Text *pcbUmText  = nullptr;
        gui::Text *pcbUmValue = nullptr;

        gui::Text *pcbLmText  = nullptr;
        gui::Text *pcbLmValue = nullptr;

        std::unique_ptr<TechnicalWindowContract::Presenter> presenter;
    };

} // namespace gui
