// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/application-settings-new/windows/LanguagesWindow.hpp>

namespace gui
{
    class OnBoardingLanguagesWindow : public LanguagesWindow
    {
      public:
        explicit OnBoardingLanguagesWindow(app::Application *app);

      private:
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} /* namespace gui */
