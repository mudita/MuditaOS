// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/AppWindow.hpp>

#include <functional>

namespace gui
{
    class BellBaseLayout;

    class BellWelcomeWindow : public AppWindow
    {
        static constexpr auto midline_correction    = 40U;
        static constexpr auto midline_w             = 480U;
        static constexpr auto midline_h             = 1U;
        static constexpr auto sun_logo_margin_top   = 10U;
        static constexpr auto sun_logo_margin_right = 25U;

      public:
        using Callback                    = std::function<void()>;
        static constexpr auto defaultName = "BellWelcomeWindow";

        explicit BellWelcomeWindow(app::ApplicationCommon *app,
                                   const std::string &name = defaultName,
                                   Callback onAction       = {});

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

      private:
        BellBaseLayout *body{nullptr};
        Callback onAction;
    };
} /* namespace gui */
