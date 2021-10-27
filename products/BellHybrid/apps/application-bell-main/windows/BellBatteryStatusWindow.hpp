// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class TextFixedSize;
    class ImageBox;
    class Image;

    class BellBatteryStatusWindow : public WindowWithTimer
    {
      public:
        struct Data : public gui::SwitchData
        {
            Data(std::uint32_t lvl, bool chargeState) : chargeLevel{lvl}, isCharging{chargeState}
            {}
            const std::uint32_t chargeLevel;
            const bool isCharging;
        };
        static constexpr auto name = "BellBatteryStatusWindow";
        explicit BellBatteryStatusWindow(app::ApplicationCommon *app);

      private:
        static constexpr auto top_description_font          = style::window::font::largelight;
        static constexpr auto bottom_description_font       = style::window::font::verybiglight;
        static constexpr auto bottom_description_max_size_w = 85U;
        static constexpr auto bottom_description_max_size_h = 85U;

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        BellBaseLayout *body{};
        TextFixedSize *topDescription{};

        TextFixedSize *bottomDescription{};
        ImageBox *chargingIcon{};

        HBox *hbox{};
        ImageBox *center{};
    };

} // namespace gui
