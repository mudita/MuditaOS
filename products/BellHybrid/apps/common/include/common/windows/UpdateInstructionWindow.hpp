// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "UpdateInstructionWindowContract.hpp"
#include <ApplicationCommon.hpp>
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class UpdateInstructionWindow : public AppWindow, public UpdateInstructionWindowContract::View
    {
        std::unique_ptr<UpdateInstructionWindowContract::Presenter> presenter;
        WidgetSpinner *spinner     = nullptr;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;

      public:
        UpdateInstructionWindow(app::ApplicationCommon *app,
                                std::unique_ptr<UpdateInstructionWindowContract::Presenter> &&presenter,
                                const std::string &name);

        [[nodiscard]] bool isLastLayout() const;
    };
} // namespace gui
