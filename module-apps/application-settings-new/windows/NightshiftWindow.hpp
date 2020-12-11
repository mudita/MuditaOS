// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "application-settings-new/widgets/timeWidget.hpp"

namespace gui
{
    class NightshiftWindow : public AppWindow
    {
      public:
        explicit NightshiftWindow(app::Application *app);

      private:
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        VBox *body                 = nullptr;
        TimeWidget *fromTimeWidget = nullptr;
        TimeWidget *toTimeWidget   = nullptr;
        Label *separatorLabel      = nullptr;
    };

} /* namespace gui */
