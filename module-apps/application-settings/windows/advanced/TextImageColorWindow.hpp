// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Text.hpp>
#include <Icon.hpp>

namespace gui
{
    class TextImageColorWindow : public AppWindow
    {
      protected:
        unsigned int colour   = 0;
        unsigned int image_nr = 1;
        Icon *testIcon        = nullptr;

      public:
        explicit TextImageColorWindow(app::ApplicationCommon *app);

        bool onInput(const InputEvent &inputEvent) override;
    };
} // namespace gui
