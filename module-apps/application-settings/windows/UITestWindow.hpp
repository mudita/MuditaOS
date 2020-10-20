// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <Text.hpp>

namespace gui
{
    class UiTestWindow : public AppWindow
    {
      protected:
        gui::Text *text = nullptr;

      public:
        UiTestWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void set_navigation();

      private:
        void invalidate() noexcept;
    };
} // namespace gui
