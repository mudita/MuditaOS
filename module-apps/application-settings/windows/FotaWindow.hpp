#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Text.hpp"

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

        Text apnName;
        Text apnUser;
        Text apnPassword;

        Label currentFirmware;
        Label availablUpdate;



    };
} //namespace gui
