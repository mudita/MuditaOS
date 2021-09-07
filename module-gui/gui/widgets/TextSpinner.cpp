// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextSpinner.hpp"

namespace gui
{
    TextSpinner::TextSpinner(const TextSpinner::TextRange &range, Boundaries boundaries)
        : textRange{range}, boundaries(boundaries)
    {
        setEditMode(EditMode::Browse);
        drawUnderline(false);
        update();
    }
    void TextSpinner::setTextRange(const TextSpinner::TextRange &range)
    {
        textRange = range;
        update();
    }
    void TextSpinner::setCurrentPosition(Position pos)
    {
        if ((pos >= getRangeDownLimit()) && (pos <= getRangeUpLimit())) {
            currentPosition = pos;
        }
        update();
    }

    UTF8 TextSpinner::getCurrentText() const noexcept
    {
        return textRange[currentPosition];
    }

    void TextSpinner::stepUp()
    {
        if (currentPosition >= getRangeUpLimit()) {
            if (boundaries == Boundaries::Continuous) {
                currentPosition = getRangeDownLimit();
            }
            else {
                currentPosition = getRangeUpLimit();
            }
        }
        else {
            currentPosition++;
        }
        update();
    }
    void TextSpinner::stepDown()
    {
        if (currentPosition <= getRangeDownLimit()) {
            if (boundaries == Boundaries::Continuous) {
                currentPosition = getRangeUpLimit();
            }
            else {
                currentPosition = getRangeDownLimit();
            }
        }
        else {
            currentPosition--;
        }
        update();
    }
    void TextSpinner::update()
    {
        setText(textRange[currentPosition]);
    }
    bool TextSpinner::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_UP:
                stepUp();
                return true;
            case KeyCode::KEY_DOWN:
                stepDown();
                return true;
            default:
                break;
            }
        }
        return false;
    }
    bool TextSpinner::onFocus(bool state)
    {
        if (focus) {
            setEdges(focusEdges);
        }
        else {
            setEdges(RectangleEdge::None);
        }
        showCursor(state);
        return true;
    }
    TextSpinner::Position TextSpinner::getRangeUpLimit() const
    {
        return static_cast<Position>(textRange.size() - 1);
    }
    TextSpinner::Position TextSpinner::getRangeDownLimit() const
    {
        return 0;
    }
    TextSpinner::Range TextSpinner::getValidRange() const noexcept
    {
        return Range{getRangeDownLimit(), getRangeUpLimit()};
    }
    void TextSpinner::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

} // namespace gui
