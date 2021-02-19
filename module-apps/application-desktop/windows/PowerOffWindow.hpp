// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "../presenter/PowerOffPresenter.hpp"

namespace gui
{
    class PowerOffWindow : public AppWindow
    {
        enum class State
        {
            PowerDown,
            Return,
        };

        gui::Label *titleLabel = nullptr;
        gui::Label *infoLabel  = nullptr;

        std::unique_ptr<PowerOffPresenter> presenter;
        std::vector<gui::Label *> selectionLabels;
        gui::Label *eventMgrLabel  = nullptr;
        gui::Image *powerImage     = nullptr;
        gui::Image *powerDownImage = nullptr;
        State state                = State::Return;
      public:
        PowerOffWindow(app::Application *app, std::unique_ptr<PowerOffPresenter> &&presenter);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void invalidate() noexcept;
    };

} /* namespace gui */
