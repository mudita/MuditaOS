// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <DialogMetadata.hpp>
#include <Text.hpp>
#include <Icon.hpp>

#include <functional>

namespace gui
{
    class MmiPushWindow : public AppWindow
    {
      protected:
        Icon *icon = nullptr;

      public:
        MmiPushWindow(app::ApplicationCommon *app, const std::string &name);
        ~MmiPushWindow() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
    };

}; // namespace gui
