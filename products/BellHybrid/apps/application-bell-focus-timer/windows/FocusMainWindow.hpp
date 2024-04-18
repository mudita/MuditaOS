// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FocusCommon.hpp"
#include <common/options/BellOptionWindow.hpp>
#include <presenter/FocusMainPresenter.hpp>

namespace app::focus
{
    using namespace gui;

    class FocusMainWindow : public BellOptionWindow, public app::focus::FocusMainContract::View
    {
      public:
        FocusMainWindow(app::ApplicationCommon *app,
                        std::unique_ptr<app::focus::FocusMainContract::Presenter> &&presenter,
                        const std::string &name = window::name::main);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        std::unique_ptr<app::focus::FocusMainContract::Presenter> presenter;
        std::list<Option> settingsOptionsList();
        std::function<void()> startFocus;
    };
} // namespace app::focus
