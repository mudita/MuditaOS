// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common/AudioErrorPresenter.hpp"
#include <apps-common/popups/WindowWithTimer.hpp>
#include <popups/data/AudioErrorParams.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class BellBaseLayout;
    class AudioErrorWindow : public WindowWithTimer
    {
      public:
        using OnExitCallback = std::function<void()>;

        AudioErrorWindow(app::ApplicationCommon *app,
                         const std::string &name,
                         std::unique_ptr<AudioErrorContract::Presenter> &&presenter,
                         OnExitCallback callback = nullptr);

      private:
        std::unique_ptr<AudioErrorContract::Presenter> presenter;
        std::string errorText;
        Icon *icon{nullptr};
        AudioErrorType errorType{AudioErrorType::Unknown};
        OnExitCallback onExitCallback;

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void registerCallbacks();
        void buildLayout();
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} // namespace gui
