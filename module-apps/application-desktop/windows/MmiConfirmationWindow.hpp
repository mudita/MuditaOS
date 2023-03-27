// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <module-apps/apps-common/popups/WindowWithTimer.hpp>
#include <DialogMetadata.hpp>
#include <Text.hpp>
#include <Icon.hpp>

#include <functional>

namespace gui
{
    class MmiConfirmationWindow : public WindowWithTimer
    {
      protected:
        Icon *icon = nullptr;

      public:
        MmiConfirmationWindow(app::ApplicationCommon *app, const std::string &name);
        ~MmiConfirmationWindow() override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
    };

}; // namespace gui
