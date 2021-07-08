// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <DialogMetadata.hpp>
#include <Text.hpp>

#include <functional>

namespace gui
{
    class Image;
    class MmiPushWindow : public AppWindow
    {
      protected:
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        MmiPushWindow(app::Application *app, const std::string &name);
        ~MmiPushWindow() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
    };

}; // namespace gui
