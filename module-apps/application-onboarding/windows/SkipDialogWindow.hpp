// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Dialog.hpp"

namespace app::onBoarding
{
    class SkipDialogWindow : public gui::DialogYesNo
    {
      public:
        explicit SkipDialogWindow(app::Application *app);

        gui::status_bar::Configuration configureStatusBar(gui::status_bar::Configuration appConfiguration) override;
    };
} // namespace app::onBoarding
