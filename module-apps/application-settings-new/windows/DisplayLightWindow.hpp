#pragma once

#include <module-apps/windows/OptionWindow.hpp>
#include "BaseSettingsWindow.hpp"
#include <i18/i18.hpp>
#include <ProgressBar.hpp>
#include <InputEvent.hpp>

#include "application-settings-new/widgets/UpDown.hpp"
#include "application-settings-new/widgets/Toggle.hpp"

namespace gui
{

    class DisplayLightWindow : public BaseSettingsWindow
    {
      public:
        DisplayLightWindow(app::Application *app);
        void buildInterface() override;

      protected:
        void invalidate() noexcept override;

      private:
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void displayLightSwitchHandler();
        void autoLightSwitchHandler();

        Toggle *displayLight = nullptr;
        Toggle *autoLight    = nullptr;
        UpDown *brightness   = nullptr;
        VBox *body           = nullptr;

        const uint32_t max_brightness     = 5;
        const uint32_t default_brightness = 3;
    };
} // namespace gui
