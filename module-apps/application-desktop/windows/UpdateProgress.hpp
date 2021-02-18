// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include "AppWindow.hpp"
#include "Dialog.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/TextFixedSize.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/ProgressBar.hpp"
namespace gui
{

    class UpdateProgressWindow : public AppWindow
    {

        enum class State
        {
            UpdateNow,
            Return,
        };

        unsigned int progressPercent = 0;

        gui::Label *percentLabel    = nullptr;
        ProgressBar *updateProgress = nullptr;

        State state = State::Return;
        fs::path updateFile;

      protected:
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        UpdateProgressWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
