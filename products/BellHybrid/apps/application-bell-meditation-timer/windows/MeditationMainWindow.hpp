// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/options/BellOptionWindow.hpp>
#include <InputEvent.hpp>
#include <presenter/MeditationBasicsPresenter.hpp>

namespace app::meditation
{
    class MeditationMainWindow : public gui::BellOptionWindow
    {
      public:
        static constexpr auto defaultName = gui::name::window::main_window;
        explicit MeditationMainWindow(app::ApplicationCommon *app,
                                      std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter);
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::list<gui::Option> settingsOptionsList();
        std::shared_ptr<app::meditation::MeditationBasicsContract::Presenter> presenter;
    };

} // namespace app::meditation
