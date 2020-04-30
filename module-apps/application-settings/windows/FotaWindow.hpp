#pragma once

#include "Fota.hpp"

#include <Application.hpp>
#include <AppWindow.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Text.hpp>
#include <gui/widgets/BoxLayout.hpp>

#include <memory>
#include <string>

namespace gui
{
    namespace window
    {
        inline const std::string fota_window = "fota_window";
    };
    /** @brief Wndow managing modem firmware update
     *
     */
    class FotaWindow : public AppWindow
    {
    public:
        FotaWindow(app::Application *app);
        virtual ~FotaWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        //        Text apnName;
        //        Text apnUser;
        //        Text apnPassword;

      private:
        void add_box_label(BoxLayout *layout, const std::string &text);
        std::unique_ptr<Fota> fotaWorker;
        Label *currentFirmwareLabel;
        Label *availablUpdateLabel;
        Label *statusLabel;

        VBox *mainBox;
    };
} //namespace gui
