// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/Spinner.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class BellMainSetHour : public AppWindow
    {
      public:
        explicit BellMainSetHour(app::Application *app);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;

      private:
        gui::Spinner *spinnerHour   = nullptr;
        gui::Spinner *spinnerMinute = nullptr;
    };
} // namespace gui
