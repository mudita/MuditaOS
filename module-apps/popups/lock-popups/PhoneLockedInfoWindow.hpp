// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Text.hpp>
#include <gui/widgets/Image.hpp>

namespace gui
{
    class PhoneLockedInfoWindow : public AppWindow
    {
        gui::Image *lockImage = nullptr;
        gui::Text *infoText   = nullptr;

        void setVisibleState();

      public:
        PhoneLockedInfoWindow(app::Application *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void buildInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
