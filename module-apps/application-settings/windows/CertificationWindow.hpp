// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class CertificationWindow : public AppWindow
    {
      public:
        explicit CertificationWindow(app::Application *app);
        ~CertificationWindow() override;
        void destroyInterface() override;

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        gui::Text *usFccIdText    = nullptr;
        gui::Text *usFccIdValue   = nullptr;
        gui::Image *usFccIdImage  = nullptr;
        gui::Text *canadaIcText   = nullptr;
        gui::Text *canadaIcValue  = nullptr;
        gui::Image *canadaIcImage = nullptr;
        gui::Text *europeCeText   = nullptr;
        gui::Text *europeCeValue  = nullptr;
        gui::Image *europeCeImage = nullptr;
    };

} // namespace gui
