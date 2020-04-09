
#pragma once

#include "Application.hpp"
#include "gui/widgets/Text.hpp"
#include "windows/AppWindow.hpp"

namespace gui
{
    namespace name
    {
        namespace window
        {
            const std::string scan_window("AntennaScanWindow");
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
        ScanModesWindow(app::Application *app);
        virtual ~ScanModesWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void updateCurrentMode(std::string &data);
    };

} /* namespace gui */
