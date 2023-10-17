// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ButtonTriState.hpp"

#include <Style.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    ButtonTriState::ButtonTriState(Item *parent, State state)
        : Label{parent}, transitingText{utils::translate("app_settings_toggle_transiting")}
    {
        switchState(state);
    }

    void ButtonTriState::switchState(State requestedState)
    {
        setFont(style::window::font::small);
        setRadius(4);
        setPenWidth(2);

        auto setRectangleStyle = [this]() {
            setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            setMinimumSize(style::buttonTriState::w, style::buttonTriState::h);
            setEdges(RectangleEdge::All);
            setCorners(RectangleRoundedCorner::All);
        };

        auto setTextOnlyStyle = [this]() {
            setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            setMinimumSize(120,
                           style::buttonTriState::h); // unfortunately, setMinimumWidthToFitText() doesn't work here
            setEdges(RectangleEdge::None);
            setCorners(RectangleRoundedCorner::None);
            setColor(ColorFullBlack);
        };

        currentState = requestedState;

        switch (currentState) {
        case State::On:
            setRectangleStyle();
            setFillColor(ColorFullBlack);
            setColor(ColorFullWhite);
            setText(utils::translate("app_settings_toggle_on"));
            break;
        case State::Transiting:
            setTextOnlyStyle();
            setText(transitingText);
            break;
        default:
            LOG_ERROR("button state '%s' not implemented - defaulting to OFF",
                      magic_enum::enum_name(currentState).data());
            [[fallthrough]];
        case State::Off:
            setRectangleStyle();
            setFillColor(ColorFullWhite);
            setColor(ColorFullBlack);
            setText(utils::translate("app_settings_toggle_off"));
            break;
        }
    }
} /* namespace gui */
