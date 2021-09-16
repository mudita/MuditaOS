// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DesktopInputWidget.hpp"

#include <AppWindow.hpp>
#include <DialogMetadata.hpp>
#include <Text.hpp>

#include <functional>

namespace gui
{
    class DesktopInputWidget;

    class MmiPullWindow : public AppWindow
    {
      protected:
        Text *text                   = nullptr;
        DesktopInputWidget *InputBox = nullptr;

      public:
        MmiPullWindow(app::ApplicationCommon *app, const std::string &name);
        ~MmiPullWindow() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
    };

}; // namespace gui
