// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BellOnBoardingNames.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellBaseLayout;

    class OnBoardingWelcomeWindow : public AppWindow
    {
      public:
        explicit OnBoardingWelcomeWindow(app::ApplicationCommon *app,
                                         const std::string &name = gui::window::name::onBoardingLanguageWindow);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

      private:
        BellBaseLayout *body{nullptr};
    };
} /* namespace gui */
