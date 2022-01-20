// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Dialog.hpp"

#include <module-apps/application-onboarding/presenter/EULALicenseWindowPresenter.hpp>

#include <module-gui/gui/widgets/text/Label.hpp>
#include <module-gui/gui/widgets/text/Text.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace app::onBoarding
{
    class NoConfigurationDialogWindow : public gui::Dialog
    {
      public:
        explicit NoConfigurationDialogWindow(app::ApplicationCommon *app);

        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
