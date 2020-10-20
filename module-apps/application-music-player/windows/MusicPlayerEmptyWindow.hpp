// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include <Text.hpp>
#include <Image.hpp>

#include <vector>
#include <string>

namespace gui
{

    class MusicPlayerEmptyWindow : public AppWindow
    {
        Image *img         = nullptr;
        Text *text         = nullptr;
        Image *placeHolder = nullptr;

      public:
        MusicPlayerEmptyWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
        bool onInput(const InputEvent &inputEvent) final;
    };

} /* namespace gui */
