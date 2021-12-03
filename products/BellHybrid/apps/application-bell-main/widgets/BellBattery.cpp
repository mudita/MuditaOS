// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBattery.hpp"
#include "data/BatteryUtils.hpp"
#include <Image.hpp>

namespace
{
    constexpr auto entries =
        std::array<battery_utils::BatteryLevelEntry, 7>{{{{0, 5}, {1, 1}, "bell_battery_empty"},
                                                         {{5, 10}, {1, 5}, "bell_battery_empty"},
                                                         {{11, 30}, {6, 29}, "bell_battery_lvl1"},
                                                         {{31, 50}, {30, 53}, "bell_battery_lvl2"},
                                                         {{51, 70}, {54, 77}, "bell_battery_lvl3"},
                                                         {{71, 95}, {78, 99}, "bell_battery_lvl4"},
                                                         {{96, 100}, {100, 100}, "bell_battery_lvl5"}}};
}

namespace gui
{
    BellBattery::BellBattery(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : HBox(parent, x, y, w, h)
    {
        img = new Image(this, battery::battery_low, gui::ImageTypeSpecifier::W_M);
        img->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        img->setMargins(gui::Margins(0, 0, battery::image_right_margin, 0));

        percentText = new TextFixedSize(this, 0, 0, 0, 0);
        percentText->setMinimumSize(battery::percent_w, battery::percent_h);
        percentText->setFont(battery::font_small);
        percentText->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        percentText->setEdges(RectangleEdge::None);
        percentText->activeItem = false;
        percentText->drawUnderline(false);
    }

    void BellBattery::update(const Store::Battery &batteryContext)
    {
        const auto result = battery_utils::getScaledBatteryLevel(entries, batteryContext.level);
        if (not result) {
            return;
        }

        const auto level = result->level;
        if (batteryContext.state == Store::Battery::State::Charging) {
            img->set(battery::battery_charging, gui::ImageTypeSpecifier::W_M);

            percentText->setText(std::to_string(level) + "%");
            setVisible(true);
        }
        else {
            if (level > 20) {
                setVisible(false);
            }
            else {
                img->set(result->image, gui::ImageTypeSpecifier::W_M);
                percentText->setText(std::to_string(level) + "%");
                setVisible(true);
            }
        }
    }
} // namespace gui
