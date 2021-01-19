// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionSetting.hpp"

#include <utility>
#include "SpinBox.hpp"

namespace gui
{
    SpinBoxOptionSettings::SpinBoxOptionSettings(UTF8 text,
                                                 std::uint8_t value,
                                                 std::uint8_t maxValue,
                                                 std::function<bool(uint8_t)> updateCallback,
                                                 std::function<bool(Item &)> focusChangedCallback)
        : option::OptionSettings(text, nullptr, focusChangedCallback, nullptr),
          updateCallback(std::move(updateCallback)), maxValue(maxValue), value(value)
    {}

    auto SpinBoxOptionSettings::build() const -> ListItem *
    {
        auto spinBox = new SpinBox(nullptr, text, updateCallback, maxValue, value);

        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        optionItem->inputCallback        = spinBox->inputCallback;
        optionItem->focusChangedCallback = [spinBox, this](Item &item) {
            spinBox->focusChangedCallback(item);
            return focusChangedCallback(item);
        };
        optionItem->dimensionChangedCallback = [spinBox](gui::Item &, const BoundingBox &newDim) -> bool {
            spinBox->setPosition(0, 0);
            spinBox->setSize(newDim.w, newDim.h);
            return true;
        };

        optionItem->addWidget(spinBox);

        return optionItem;
    }
} // namespace gui
