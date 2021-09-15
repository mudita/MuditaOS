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
        IconTextWindow(app::Application *app, std::string name);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        void destroyInterface() override;

        virtual std::chrono::seconds getTimeout() const = 0;
        virtual std::string getText()                   = 0;
        virtual std::string getImageName()              = 0;

      private:
        gui::Image *appImage = nullptr;
        gui::Text *text      = nullptr;

        void showImage();
        void showText();
    };
} // namespace gui
