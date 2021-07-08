// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/TextFixedSize.hpp>

#include <vector>

namespace gui
{
    class UpdateWindow : public AppWindow
    {

        enum class State
        {
            UpdateNow,
            Return,
        };

        gui::Label *titleLabel   = nullptr;
        gui::Label *infoLabel    = nullptr;
        gui::Label *detailLabel  = nullptr;
        gui::Label *percentLabel = nullptr;

        gui::Label *currentVersionInfo = nullptr;
        gui::Label *updateVersionInfo  = nullptr;
        gui::Label *updateDetails      = nullptr;

        std::vector<gui::Label *> selectionLabels;
        State state         = State::Return;
        int progressPercent = 0;

      public:
        UpdateWindow(app::Application *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        fs::path updateFile;
    };

} /* namespace gui */
