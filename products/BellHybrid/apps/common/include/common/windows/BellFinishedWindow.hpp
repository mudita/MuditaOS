// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/data/BellFinishedWindowSwitchData.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    namespace window::bell_finished
    {
        constexpr inline auto defaultName = "BellFinishedWindow";
    } // namespace window::bell_finished

    class Icon;

    class BellFinishedWindow : public WindowWithTimer
    {
      public:
        explicit BellFinishedWindow(app::ApplicationCommon *app,
                                    const std::string &name = window::bell_finished::defaultName);

      protected:
        void buildInterface() override;
        void exit();
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};
        std::string windowToReturn;
        bool closeApplication; // is close application is true windowToReturn variable is ignored
    };

} // namespace gui
