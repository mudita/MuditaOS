// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBattery.hpp"
#include <Image.hpp>

namespace
{
    constexpr unsigned int minVal   = 80;
    constexpr unsigned int oldMax   = 95;
    constexpr unsigned int newMax   = 100;
    constexpr unsigned int oldRange = oldMax - minVal;
    constexpr unsigned int newRange = newMax - minVal;

    unsigned int translateBatteryLevel(unsigned int percentage)
    {
        return (((percentage - minVal) * newRange) / oldRange) + minVal;
    }
} // namespace

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
        (void)batteryContext;

        // Fuel gauge and charger are different hw elements
        // Charger sometimes stops charging before FG shows 100%
        auto level = batteryContext.level;

        if (level > 95) {
            level = 100;
        }

        // Translate 80-95% range to 80-100% range for display
        if ((level > 80) && (level <= 95)) {
            level = translateBatteryLevel(level);
        }

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
                if (level > 10) {
                    img->set(battery::battery_low, gui::ImageTypeSpecifier::W_M);
                }
                else {
                    img->set(battery::battery_critical, gui::ImageTypeSpecifier::W_M);
                }
                percentText->setText(std::to_string(level) + "%");
                setVisible(true);
            }
        }
    }
} // namespace gui
