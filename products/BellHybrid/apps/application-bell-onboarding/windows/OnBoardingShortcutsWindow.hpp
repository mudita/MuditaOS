// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/OnBoardingShortcutsWindowPresenter.hpp"
#include <ApplicationCommon.hpp>
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class SideListView;
    class OnBoardingShortcutsWindow : public AppWindow, public app::OnBoarding::OnBoardingShortcutsWindowContract::View
    {
        std::unique_ptr<app::OnBoarding::OnBoardingShortcutsWindowContract::Presenter> presenter;
        SideListView *sideListView = nullptr;
        WidgetSpinner *spinner     = nullptr;

        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;

        void onValueChanged(const std::uint32_t currentValue);

      public:
        OnBoardingShortcutsWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::OnBoarding::OnBoardingShortcutsWindowContract::Presenter> &&presenter,
            const std::string &name);

        bool isOneOfTwoLastShortcuts() const;
    };
} // namespace gui
