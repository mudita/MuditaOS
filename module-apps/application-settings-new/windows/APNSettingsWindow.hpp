// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include <Icon.hpp>

namespace gui
{
    class APNSettingsWindow : public OptionWindow
    {
      public:
        APNSettingsWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        Image *leftArrowImage = nullptr;
        Image *crossImage     = nullptr;
        Icon *emptyListIcon   = nullptr;
    };
}; // namespace gui
