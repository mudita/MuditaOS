// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

#include <module-apps/application-onboarding/presenter/EULALicenseWindowPresenter.hpp>

#include <module-gui/gui/widgets/text/Label.hpp>
#include <module-gui/gui/widgets/text/Text.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace app::onBoarding
{
    class EULALicenseWindow : public gui::AppWindow, public EULALicenseWindowContract::View
    {
      public:
        explicit EULALicenseWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<EULALicenseWindowContract::Presenter> &&windowPresenter);
        ~EULALicenseWindow() noexcept override;

        gui::status_bar::Configuration configureStatusBar(gui::status_bar::Configuration appConfiguration) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        std::unique_ptr<EULALicenseWindowContract::Presenter> presenter;
        gui::Text *eulaText = nullptr;
    };
} // namespace app::onBoarding
