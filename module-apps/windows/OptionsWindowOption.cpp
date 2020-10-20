// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsWindowOption.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <cassert>

namespace style::option
{
    const gui::Position arrow_position_x = 408;
    const gui::Position arrow_position_y = 24;
} // namespace style::option

namespace gui::option
{
    auto Simple::build() const -> Item *
    {
        auto *label = new gui::Label(nullptr,
                                     style::window::default_left_margin,
                                     0,
                                     style::window_width - 2 * style::window::default_left_margin,
                                     style::window::label::big_h,
                                     text);
        style::window::decorateOption(label);
        label->activatedCallback = activatedCallback;
        if (arrow == Arrow::Enabled) {
            new gui::Image(
                label, style::option::arrow_position_x, style::option::arrow_position_y, 0, 0, "right_label_arrow");
        }
        return label;
    }

} // namespace gui::option

namespace gui
{
    [[nodiscard]] auto Option::build() const -> Item *
    {
        assert(option);
        return option->build();
    }
} // namespace gui
