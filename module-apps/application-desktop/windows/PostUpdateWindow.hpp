// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <module-gui/gui/widgets/Image.hpp>

namespace gui
{
    class PostUpdateWindow : public AppWindow
    {
        gui::Image *successImage = nullptr;
        gui::Text *infoText      = nullptr;

        void setVisibleState();
        void invalidate() noexcept;

        std::string currentOsVersion;

      public:
        explicit PostUpdateWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
