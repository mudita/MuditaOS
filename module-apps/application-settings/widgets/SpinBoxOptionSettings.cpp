// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpinBoxOptionSettings.hpp"

#include <widgets/SpinBox.hpp>

namespace gui
{
    SpinBoxOptionSettings::SpinBoxOptionSettings(UTF8 text,
                                                 std::uint8_t value,
                                                 std::uint8_t maxValue,
                                                 std::function<bool(uint8_t)> updateCallback,
                                                 std::function<void(const UTF8 &text)> navBarTemporaryMode,
                                                 std::function<void()> navBarRestoreFromTemporaryMode,
                                                 bool indent)
        : updateCallback(std::move(updateCallback)), navBarTemporaryMode(std::move(navBarTemporaryMode)),
          navBarRestoreFromTemporaryMode(std::move(navBarRestoreFromTemporaryMode)), maxValue(maxValue), value(value),
          text(text), indent(indent)
    {}

    auto SpinBoxOptionSettings::build() const -> ListItem *
    {
        auto spinBox = new SpinBox(
            nullptr, text, updateCallback, maxValue, value, navBarTemporaryMode, navBarRestoreFromTemporaryMode);

        auto optionItem = new gui::ListItem();
        optionItem->setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        optionItem->inputCallback            = spinBox->inputCallback;
        optionItem->focusChangedCallback     = spinBox->focusChangedCallback;
        optionItem->dimensionChangedCallback = [spinBox](gui::Item &, const BoundingBox &newDim) -> bool {
            spinBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        if (indent) {
            optionItem->setMargins(Margins(option::window::option_left_margin, 0, 0, 0));
        }

        optionItem->addWidget(spinBox);

        return optionItem;
    }
} // namespace gui
