// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "TemperatureUnitListItem.hpp"

#include <gui/widgets/TextSpinner.hpp>
#include <module-utils/Temperature.hpp>

namespace gui
{
    TemperatureUnitListItem::TemperatureUnitListItem(const UTF8 &topDesc, Length x, Length y, Length w, Length h)
        : BellSideListItem(topDesc)
    {
        setMinimumSize(style::sidelistview::list_item::w, style::sidelistview::list_item::h);
        setEdges(RectangleEdge::None);
        setFocusItem(body);

        temperatureUnit =
            new TextSpinner({utils::temperature::celsiusDegreeSymbol, utils::temperature::fahrenheitDegreeSymbol},
                            Boundaries::Continuous);
        temperatureUnit->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        temperatureUnit->setFont(bell_settings_style::time_fmt_set_list_item::font);
        temperatureUnit->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        temperatureUnit->setFocusEdges(RectangleEdge::None);

        body->getCenterBox()->addWidget(temperatureUnit);

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
        focusChangedCallback = [&](Item &) {
            setFocusItem(focus ? body : nullptr);
            return true;
        };

        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool { return body->onInput(inputEvent); };
    }

    auto TemperatureUnitListItem::getUnitAsStr() const noexcept -> UTF8
    {
        return temperatureUnit->getCurrentText();
    }
} // namespace gui
