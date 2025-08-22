// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Text;
    class ImageBox;
    class Image;

    class BellBatteryStatusWindow : public WindowWithTimer
    {
      public:
        static constexpr auto windowName = "BellBatteryStatusWindow";

        explicit BellBatteryStatusWindow(app::ApplicationCommon *app, const std::string &name = windowName);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;

        BellBaseLayout *body{};
        Text *topDescription{};

        Text *bottomDescription{};
        ImageBox *chargingIcon{};

        HBox *hbox{};
        ImageBox *batteryImage{};
    };

} // namespace gui
