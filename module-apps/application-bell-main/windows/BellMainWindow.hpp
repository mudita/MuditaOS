// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenter/BellAlarmPresenter.hpp>

#include <AppWindow.hpp>
#include <gui/widgets/Spinner.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class BellMainWindow : public AppWindow, public BellAlarmWindowContract::View
    {
      public:
        explicit BellMainWindow(app::Application *app,
                                std::unique_ptr<BellAlarmWindowContract::Presenter> &&windowPresenter);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool updateTime() override;

        gui::Label *time        = nullptr;
        gui::Label *temperature = nullptr;

      private:
        std::unique_ptr<BellAlarmWindowContract::Presenter> presenter;
    };
} // namespace gui
