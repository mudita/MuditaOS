// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/BGSoundsTimerSelectPresenter.hpp"
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{

    class BellBaseLayout;
    class Text;

    class BGSoundsTimerSelectWindow : public AppWindow, public app::bgSounds::BGSoundsTimerSelectContract::View
    {
        std::unique_ptr<app::bgSounds::BGSoundsTimerSelectContract::Presenter> presenter;
        BellBaseLayout *body    = nullptr;
        UTF8Spinner *spinner    = nullptr;
        Text *bottomDescription = nullptr;

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void createTopMessage();
        void createSpinner();
        void createBottomDescription();
        void updateBottomDescription();
        void registerCallbacks();

      public:
        BGSoundsTimerSelectWindow(app::ApplicationCommon *app,
                                  std::unique_ptr<app::bgSounds::BGSoundsTimerSelectContract::Presenter> &&presenter);
    };

} // namespace gui
