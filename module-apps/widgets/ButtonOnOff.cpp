// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ButtonOnOff.hpp"

#include <Style.hpp>
#include <i18/i18.hpp>

namespace style
{
    namespace buttonOnOff
    {
        constexpr uint32_t w = 56;
        constexpr uint32_t h = 32;
    } // namespace buttonOnOff
} // namespace style

namespace gui
{
    ButtonOnOff::ButtonOnOff(Item *parent, ButtonType buttonType) : Label{parent}
    {
        setMinimumSize(style::buttonOnOff::w, style::buttonOnOff::h);

        setEdges(RectangleEdge::All);
        setCorners(RectangleRoundedCorner::All);
        setRadius(4);
        setPenWidth(2);
        setFilled(true);
        setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        if (buttonType == ButtonType::On) {
            setFillColor(ColorFullBlack);
            setText(utils::translateI18("app_settings_toggle_on"));
            setTextColor(ColorFullWhite);
        }
        else if (buttonType == ButtonType::Off) {
            setFillColor(ColorFullWhite);
            setText(utils::translateI18("app_settings_toggle_off"));
            setTextColor(ColorFullBlack);
        }
    }

} /* namespace gui */
