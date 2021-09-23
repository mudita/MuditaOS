// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/data/BellFinishedCallbackWindowSwitchData.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellFinishedCallbackWindow : public WindowWithTimer
    {
      public:
        static constexpr auto defaultName = "BellFinishedCallbackWindow";

        explicit BellFinishedCallbackWindow(app::ApplicationCommon *app, const std::string &name = defaultName);

      protected:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};
        std::function<void()> finishCallback = nullptr;
    };

} // namespace gui
