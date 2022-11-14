// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/RelaxationTimerSelectPresenter.hpp"
#include "data/RelaxationAudioData.hpp"
#include <AppWindow.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{

    class BellBaseLayout;
    class Text;

    class RelaxationTimerSelectWindow : public AppWindow, public app::relaxation::RelaxationTimerSelectContract::View
    {
        std::unique_ptr<app::relaxation::RelaxationTimerSelectContract::Presenter> presenter;
        BellBaseLayout *body    = nullptr;
        StringSpinner *spinner  = nullptr;
        Text *bottomDescription = nullptr;
        std::unique_ptr<RelaxationAudioContext> audioContext;

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void createTopMessage();
        void createSpinner();
        void createBottomDescription();
        void updateBottomDescription();
        void registerCallbacks();

      public:
        RelaxationTimerSelectWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::relaxation::RelaxationTimerSelectContract::Presenter> &&presenter);
    };

} // namespace gui
