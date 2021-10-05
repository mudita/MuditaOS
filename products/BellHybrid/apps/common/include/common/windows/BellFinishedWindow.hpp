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
        constexpr inline auto defaultName         = "BellFinishedWindow";
        constexpr inline auto image_top_margin    = 112U;
        constexpr inline auto image_bottom_margin = 30U;
    } // namespace window::bell_finished

    class Icon;

    class BellFinishedWindow : public WindowWithTimer
    {
      public:
        explicit BellFinishedWindow(app::ApplicationCommon *app,
                                    const std::string &name = window::bell_finished::defaultName);

      protected:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};
        std::string windowToReturn;
    };

} // namespace gui
