// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InputEvent.hpp>
#include <Utils.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <Text.hpp>

#include "WithTimerWindow.hpp"

namespace gui
{
    class IconTextWindow : public WithTimerWindow
    {
      public:
        explicit IconTextWindow(app::Application *app, std::string name);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        void destroyInterface() override;

        virtual uint32_t getTimeout()              = 0;
        virtual std::vector<std::string> getText() = 0;

      private:
        gui::Image *appImage = nullptr;
        gui::Text *text      = nullptr;

        void showText();
    };
} // namespace gui
