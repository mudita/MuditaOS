// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/AppWindow.hpp>

#include <functional>

namespace gui
{
    class BellBaseLayout;

    class BellWelcomeWindowBase : public AppWindow
    {
        static constexpr auto midline_correction = 40U;
        static constexpr auto midline_w          = 480U;
        static constexpr auto midline_h          = 1U;

      public:
        using Callback                    = std::function<void()>;

        explicit BellWelcomeWindowBase(app::ApplicationCommon *app,
                                       const std::string &name,
                                       const std::string &icon,
                                       const std::string &message,
                                       Callback onAction = {});

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;

      private:
        BellBaseLayout *body{nullptr};
        std::string icon;
        std::string message;
        Callback onAction;
    };

    class BellWelcomeWindow : public BellWelcomeWindowBase
    {
      public:
        static constexpr auto name = "BellWelcomeWindow";

        explicit BellWelcomeWindow(app::ApplicationCommon *app, Callback onAction = {});
    };

    class BellChargeWelcomeWindow : public BellWelcomeWindowBase
    {
      public:
        static constexpr auto name = "BellChargeWelcomeWindow";

        explicit BellChargeWelcomeWindow(app::ApplicationCommon *app, Callback onAction = {});
    };
} /* namespace gui */
