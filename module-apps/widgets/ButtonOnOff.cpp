// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ButtonOnOff.hpp"

#include <Style.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    ButtonOnOff::ButtonOnOff(Item *parent, const ButtonState buttonState) : Label{parent}
    {
        setMinimumSize(style::buttonOnOff::w, style::buttonOnOff::h);

        setEdges(RectangleEdge::All);
        setCorners(RectangleRoundedCorner::All);
        setRadius(4);
        setPenWidth(2);
        setFilled(true);
        setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        setFont(style::window::font::small);
        switchState(buttonState);
    }

    void ButtonOnOff::switchState(const ButtonState newButtonState)
    {
        currentState = newButtonState;
        if (currentState == ButtonState::On) {
            setFillColor(ColorFullBlack);
            setText(utils::translate("app_settings_toggle_on"));
            setTextColor(ColorFullWhite);
        }
        else if (currentState == ButtonState::Off) {
            setFillColor(ColorFullWhite);
            setText(utils::translate("app_settings_toggle_off"));
            setTextColor(ColorFullBlack);
        }
    }
    void ButtonOnOff::toggleState()
    {
        switchState(static_cast<ButtonState>(!static_cast<bool>(currentState)));
    }

} /* namespace gui */
