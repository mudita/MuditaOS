// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Icon.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{
    class PostUpdateWindow : public AppWindow
    {
        Icon *infoIcon = nullptr;

        void setVisibleState();
        std::string currentOsVersion;

      public:
        explicit PostUpdateWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
