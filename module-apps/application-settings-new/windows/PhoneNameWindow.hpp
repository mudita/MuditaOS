// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once
#include "AppWindow.hpp"
#include <Text.hpp>

namespace gui
{
    class PhoneNameWindow : public AppWindow
    {
      public:
        explicit PhoneNameWindow(app::Application *app);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        Text *inputField = nullptr;
    };

} /* namespace gui */
