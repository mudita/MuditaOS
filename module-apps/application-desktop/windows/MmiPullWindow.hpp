// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-desktop/widgets/DesktopInputWidget.hpp"

#include <AppWindow.hpp>
#include <Text.hpp>
#include <functional>
#include <DialogMetadata.hpp>

namespace gui
{
    class DesktopInputWidget;

    class MmiPullWindow : public AppWindow
    {
      protected:
        Text *text                   = nullptr;
        DesktopInputWidget *InputBox = nullptr;

      public:
        MmiPullWindow(app::Application *app, const std::string &name);
        ~MmiPullWindow() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
        std::string removePhrase(std::string str, std::string phrase);
    };

}; // namespace gui
