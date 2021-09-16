// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Spinner.hpp"

#include <InputEvent.hpp>

#include <sstream>
#include <iomanip>

namespace gui
{

    Spinner::Spinner(int minValue, int maxValue, int step, Boundaries boundaries, OnUpdateCallback cb)
        : minValue(minValue), maxValue(maxValue), step(step), currentValue(minValue),
          boundaries(boundaries), onUpdate{std::move(cb)}
    {
        setEditMode(EditMode::Browse);
        drawUnderline(false);
        updateSpinner();
    }

    void Spinner::setCurrentValue(int newCurrentValue)
    {
        currentValue = newCurrentValue;
        updateSpinner();
    }

    void Spinner::setFixedFieldWidth(unsigned char newFixedFieldWidth)
    {
        fixedFieldWidth = newFixedFieldWidth;
    }

    int Spinner::getCurrentValue() const noexcept
    {
        return currentValue;
    }

    unsigned char Spinner::getFixedFieldWidth() const noexcept
    {
        return fixedFieldWidth;
    }

    void Spinner::stepUp()
    {
        currentValue += step;
        if (currentValue > maxValue) {
            if (boundaries == Boundaries::Continuous) {
                currentValue = minValue;
            }
            else {
                currentValue = maxValue;
            }
        }
        updateSpinner();
    }

    void Spinner::stepDown()
    {
        currentValue -= step;
        if (currentValue < minValue) {
            if (boundaries == Boundaries::Continuous) {
                currentValue = maxValue;
            }
            else {
                currentValue = minValue;
            }
        }
        updateSpinner();
    }

    bool Spinner::onInput(const InputEvent &inputEvent)
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

    bool Spinner::onFocus(bool state)
    {
        if (focus) {
            setEdges(focusEdges);
        }
        else {
            setEdges(RectangleEdge::None);
        }
        showCursor(state);
        updateSpinner();
        return true;
    }

    void Spinner::updateSpinner()
    {
        std::stringstream outStream;
        if (fixedFieldWidth > 0) {
            outStream << std::setw(fixedFieldWidth) << std::setfill('0');
        }
        outStream << currentValue;
        setText(outStream.str());
        if (onUpdate) {
            onUpdate(currentValue);
        }
    }
    void Spinner::setMinValue(int newMinValue)
    {
        minValue = newMinValue;
    }
    void Spinner::setMaxValue(int newMaxValue)
    {
        maxValue = newMaxValue;
    }
    void Spinner::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

} // namespace gui
