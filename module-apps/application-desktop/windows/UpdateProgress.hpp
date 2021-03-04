// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include <module-apps/windows/AppWindow.hpp>
#include <module-apps/windows/Dialog.hpp>
#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/TextFixedSize.hpp>
#include <module-gui/gui/widgets/Image.hpp>
#include <module-gui/gui/widgets/BottomBar.hpp>
#include <module-gui/gui/widgets/ProgressBar.hpp>
namespace gui
{

    class UpdateProgressWindow : public AppWindow
    {
        unsigned int progressPercent = 0;
        std::string textInfo;

        gui::Label *percentLabel    = nullptr;
        ProgressBar *updateProgress = nullptr;

        fs::path updateFile;

        Text *text  = nullptr;
        Image *icon = nullptr;

        void setVisibleState();
        void invalidate() noexcept;

      public:
        UpdateProgressWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };

} /* namespace gui */
