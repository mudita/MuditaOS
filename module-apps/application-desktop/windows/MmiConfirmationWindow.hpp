// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Icon.hpp>

namespace gui
{
    class MmiConfirmationWindow : public AppWindow
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
