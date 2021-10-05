// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/windows/BellFinishedWindow.hpp>

namespace gui
{
    namespace OnBoarding::Information
    {
        inline constexpr auto icon_top_margin = 40U;
        inline constexpr auto text_w          = 380U;
    } // namespace OnBoarding::Information

    class OnBoardingInstructionPromptWindow : public BellFinishedWindow
    {
      public:
        OnBoardingInstructionPromptWindow(app::ApplicationCommon *app, const std::string &name);

        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
    };

} // namespace gui
