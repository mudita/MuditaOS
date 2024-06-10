// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WhatsNewCommon.hpp"
#include <presenter/WhatsNewPresenter.hpp>

#include <apps-common/windows/AppWindow.hpp>

namespace app::whatsNew
{
    using namespace gui;

    class WhatsNewWindow : public gui::AppWindow, public WhatsNewContract::View
    {
      public:
        WhatsNewWindow(app::ApplicationCommon *app,
                       std::unique_ptr<WhatsNewContract::Presenter> &&presenter,
                       const std::string &name = window::name::main);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::unique_ptr<WhatsNewContract::Presenter> presenter;
    };
} // namespace app::whatsNew
