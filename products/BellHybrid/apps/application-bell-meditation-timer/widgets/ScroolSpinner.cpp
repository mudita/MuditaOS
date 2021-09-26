// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScroolSpinner.hpp"

#include <InputEvent.hpp>

#include <gsl/assert>

namespace gui
{

    ScroolSpinner::ScroolSpinner(int size,
                                 Boundaries boundaries,
                                 OnGetTitleCallback getTitleCB,
                                 OnUpdateCallback updateCB)
        : size(size), boundaries(boundaries), onGetTitle{std::move(getTitleCB)}, onUpdate{std::move(updateCB)}
    {
        Expects(onGetTitle != nullptr);

        setEditMode(EditMode::Browse);
        drawUnderline(false);
        updateScroolSpinner();
    }

    void ScroolSpinner::setCurrentIndex(int newCurrentIndex)
    {
        currentIndex = newCurrentIndex;
        if (newCurrentIndex >= size) {
            currentIndex = size - 1;
        }
        else if (currentIndex < 0) {
            currentIndex = 0;
        }
        updateScroolSpinner();
    }

    void ScroolSpinner::setFixedFieldWidth(unsigned char newFixedFieldWidth)
    {
        fixedFieldWidth = newFixedFieldWidth;
    }

    int ScroolSpinner::getCurrentIndex() const noexcept
    {
        return currentIndex;
    }

    unsigned char ScroolSpinner::getFixedFieldWidth() const noexcept
    {
        return fixedFieldWidth;
    }

    void ScroolSpinner::stepUp()
    {
        currentIndex += 1;
        if (currentIndex == size) {
            currentIndex = 0;
        }
        updateScroolSpinner();
    }

    void ScroolSpinner::stepDown()
    {
        currentIndex -= 1;
        if (currentIndex < 0) {
            currentIndex = size - 1;
        }
        updateScroolSpinner();
    }

    bool ScroolSpinner::onInput(const InputEvent &inputEvent)
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

    bool ScroolSpinner::onFocus(bool state)
    {
        if (focus) {
            setEdges(focusEdges);
        }
        else {
            setEdges(RectangleEdge::None);
        }
        showCursor(state);
        updateScroolSpinner();
        return true;
    }

    void ScroolSpinner::updateScroolSpinner()
    {
        setText(onGetTitle(currentIndex));
        if (onUpdate) {
            onUpdate(currentIndex);
        }
    }

    void ScroolSpinner::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

} // namespace gui
