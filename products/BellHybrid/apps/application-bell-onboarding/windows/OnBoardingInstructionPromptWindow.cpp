// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingInstructionPromptWindow.hpp"

#include <InputEvent.hpp>
#include <Icon.hpp>
#include <Application.hpp>

namespace gui
{
    OnBoardingInstructionPromptWindow::OnBoardingInstructionPromptWindow(app::ApplicationCommon *app,
                                                                         const std::string &name)
        : BellFinishedWindow(app, name)
    {
        buildInterface();
    }

    void OnBoardingInstructionPromptWindow::buildInterface()
    {
        BellFinishedWindow::buildInterface();

        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins({0, OnBoarding::Information::icon_top_margin, 0, 0});
        icon->text->setMaximumWidth(OnBoarding::Information::text_w);
    }

    bool OnBoardingInstructionPromptWindow::onInput(const InputEvent &inputEvent)
    {
        // Inputs are handled on Application level
        return false;
    }
} // namespace gui
