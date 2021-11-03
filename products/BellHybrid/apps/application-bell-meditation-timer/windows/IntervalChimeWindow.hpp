// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <widgets/BellBaseLayout.hpp>

#include <Application.hpp>
#include <AppWindow.hpp>
#include <InputEvent.hpp>

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

        void onValueChanged(const std::string currentValue);

      private:
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> presenter;
        BellBaseLayout *body{};
        UTF8Spinner *spinner{};
        Label *bottomDescription{};
    };
} // namespace gui
