// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{
    namespace name
    {
        namespace window
        {
            inline constexpr auto scan_window = "AntennaScanWindow";
        }
    } // namespace name
    class ScanModesWindow : public AppWindow
    {
      protected:
        std::map<uint32_t, std::string> modeButtonParams;
        enum scanModes
        {
            Auto = 0,
            GSM_only,
            WCDMA_only,
            LTE_only,
            TD_SCDMA_only,
            UTMS_only,
            CDMA_only,
            HDR_only,
            CDMA_and_HDR_only
        };

        gui::VBox *modesBox       = nullptr;
        gui::Label *commandResult = nullptr;

        gui::Label *addMode(gui::Item *parent, const UTF8 &text);
        gui::Label *addSpacer(void);

      public:
        ScanModesWindow(app::ApplicationCommon *app);

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void updateCurrentMode(std::string &data);
    };

} /* namespace gui */
