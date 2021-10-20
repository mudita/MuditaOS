// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>
#include <SideListView.hpp>

#include "IntervalChimePresenter.hpp"

namespace gui
{
    class IntervalChimeWindow : public AppWindow, public app::meditation::IntervalChimeContract::View
    {
      public:
        explicit IntervalChimeWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter);

        // virtual methods
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> presenter;
        SideListView *sideListView = nullptr;
    };
} // namespace gui
