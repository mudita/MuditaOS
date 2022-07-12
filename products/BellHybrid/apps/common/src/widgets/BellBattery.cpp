// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/data/BatteryUtils.hpp>
#include <common/widgets/BellBattery.hpp>
#include <Image.hpp>

namespace
{
    constexpr auto battery_low      = "bell_battery_lvl1";
    constexpr auto battery_charging = "bell_battery_charging";

    constexpr auto entries = std::array<battery_utils::BatteryLevelEntry, 6>{{{{0, 9}, "bell_battery_empty"},
                                                                              {{10, 19}, "bell_battery_lvl1"},
                                                                              {{20, 39}, "bell_battery_lvl2"},
                                                                              {{40, 69}, "bell_battery_lvl3"},
                                                                              {{70, 95}, "bell_battery_lvl4"},
                                                                              {{96, 100}, "bell_battery_lvl5"}}};
} // namespace

namespace gui
{
    BellBattery::BellBattery(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : HBox(parent, x, y, w, h)
    {
        img = new Image(this, battery_low, gui::ImageTypeSpecifier::W_M);
        img->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        img->setMargins(gui::Margins(0, 0, battery::image_right_margin, 0));

        percentText = new TextFixedSize(this, 0, 0, 0, 0);
        percentText->setMinimumSize(battery::percent_w, battery::percent_h);
        percentText->setFont(battery::font_small);
        percentText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        percentText->setEdges(RectangleEdge::None);
        percentText->activeItem = false;
        percentText->drawUnderline(false);
        percentText->setVisible(false);
    }

    void BellBattery::update(const units::SOC soc, const bool isCharging)
    {
        const auto image = battery_utils::getBatteryLevelImage(entries, soc);
        if (not image) {
            return;
        }

        percentText->setText(std::to_string(soc) + "%");

        if (isCharging) {
            img->set(battery_charging, gui::ImageTypeSpecifier::W_M);
        }
        else {
            img->set(image->data(), gui::ImageTypeSpecifier::W_M);
        }

        if (batteryPercentMode == BatteryPercentMode::Show) {
            percentText->setVisible(true);
        }
        else {
            percentText->setVisible(false);
        }
        img->informContentChanged();
    }

    void BellBattery::setBatteryPercentMode(BatteryPercentMode mode)
    {
        batteryPercentMode = mode;
        if (batteryPercentMode == BatteryPercentMode::Hide) {
            percentText->setVisible(false);
        }
    }
} // namespace gui
