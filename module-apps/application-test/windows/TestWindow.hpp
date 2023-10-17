// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Icon.hpp"
#include <AppWindow.hpp>

namespace gui
{
    class TestPresenter;

    class TestMainWindow : public AppWindow
    {
      private:
        TestPresenter &presenter;
        Icon *button = nullptr;

      public:
        TestMainWindow(app::ApplicationCommon *app, const std::string &name, TestPresenter &presenter);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override{};

        void changeButtonText(const UTF8 &text);
    };

}; // namespace gui
