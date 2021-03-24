// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{
    class TechnicalInformationWindow : public AppWindow
    {
      public:
        TechnicalInformationWindow(app::Application *app);
        ~TechnicalInformationWindow() override;
        void destroyInterface() override;

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        gui::Text *modelText  = nullptr;
        gui::Text *modelValue = nullptr;

        gui::Text *serialNumberText  = nullptr;
        gui::Text *serialNumberValue = nullptr;

        gui::Text *osVersionText  = nullptr;
        gui::Text *osVersionValue = nullptr;

        gui::Text *imeiText  = nullptr;
        gui::Text *imeiValue = nullptr;
    };

} // namespace gui
