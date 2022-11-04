// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ButtonTriState.hpp"

#include <Style.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    ButtonTriState::ButtonTriState(Item *parent, State state) : Label{parent}
    {
        setMinimumSize(style::buttonTriState::w, style::buttonTriState::h);
        setEdges(RectangleEdge::None);

        setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        setFont(style::window::font::small);

        fill = new Rect(this, 0, 0, 0, 0);
        fill->setEdges(RectangleEdge::All);
        fill->setCorners(RectangleRoundedCorner::All);
        fill->setRadius(4);
        fill->setPenWidth(2);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            fill->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        switchState(state);
    }

    void ButtonTriState::switchState(State requestedState)
    {
        currentState = requestedState;
        if (currentState == State::On) {
            fill->setFillColor(ColorFullBlack);
            setColor(ColorFullWhite);
            setText(utils::translate("app_settings_toggle_on"));
        }
        else {
            fill->setFillColor(ColorFullWhite);
            setColor(ColorFullBlack);
            setText(utils::translate("app_settings_toggle_off"));
        }
    }
} /* namespace gui */
