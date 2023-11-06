// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        setPenWidth(style::buttonTriState::penWidth);

        const auto setRectangleStyle = [this](State state) {
            setFont(style::window::font::small);
            setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            setEdges(RectangleEdge::All);
            setCorners(RectangleRoundedCorner::All);
            setRadius(style::buttonTriState::cornerRadiusRectangleView);
            setMinimumSize(style::buttonTriState::w, style::buttonTriState::h);
            setFillColor(state == State::On ? ColorFullBlack : ColorFullWhite);
            setColor(state == State::On ? ColorFullWhite : ColorFullBlack);
        };

        const auto setTextOnlyStyle = [this](const std::string &text) {
            setFont(style::window::font::verysmall);
            setAlignment(Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
            setEdges(RectangleEdge::None);
            setCorners(RectangleRoundedCorner::None);

            /* This needs to be changed even though the corners aren't visible
             * here - it seems setMinimumWidthToFitText() completely ignores radius
             * setting, what causes the text to not fit properly when radius != 0. */
            setRadius(style::buttonTriState::cornerRadiusTextView);

            setMinimumHeight(style::buttonTriState::h);
            setMinimumWidthToFitText(text);
            setColor(ColorFullBlack);
        };

        currentState = requestedState;

        switch (currentState) {
        case State::On:
            setRectangleStyle(State::On);
            setText(utils::translate("app_settings_toggle_on"));
            break;
        case State::Transiting:
            setTextOnlyStyle(transitingText);
            setText(transitingText);
            break;
        default:
            LOG_ERROR("Button state '%s' not implemented - defaulting to OFF",
                      magic_enum::enum_name(currentState).data());
            [[fallthrough]];
        case State::Off:
            setRectangleStyle(State::Off);
            setText(utils::translate("app_settings_toggle_off"));
            break;
        }
    }
} /* namespace gui */
