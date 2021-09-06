// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SpinnerPolicies.hpp"
#include <widgets/TextFixedSize.hpp>

namespace gui
{
    template <typename Policy> class GenericSpinner : public TextFixedSize
    {
      public:
        using Range = typename Policy::Range;
        using Type  = typename Policy::Type;

        explicit GenericSpinner(Range range,
                                Boundaries boundaries   = Boundaries::Continuous,
                                Orientation orientation = Orientation::Vertical);

        [[nodiscard]] Type getCurrentValue() const noexcept;
        void setCurrentValue(Type val);
        void setRange(Range range);

        void setFocusEdges(RectangleEdge edges);
        bool onInput(const InputEvent &inputEvent) override;
        bool onFocus(bool state) override;

      private:
        void stepNext();
        void stepPrevious();
        bool isPreviousEvent(const InputEvent &inputEvent);
        bool isNextEvent(const InputEvent &inputEvent);
        void update();

        Policy policy;
        RectangleEdge focusEdges = RectangleEdge::Bottom;
        Orientation orientation  = Orientation::Vertical;
    };

    template <typename ValuePolicy>
    GenericSpinner<ValuePolicy>::GenericSpinner(GenericSpinner::Range range,
                                                Boundaries boundaries,
                                                Orientation orientation)
        : policy{range, boundaries}, orientation(orientation)
    {
        setEditMode(EditMode::Browse);
        drawUnderline(false);
        update();
    }

    template <typename ValuePolicy> void GenericSpinner<ValuePolicy>::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

    template <typename Policy> void GenericSpinner<Policy>::setRange(Range range)
    {
        policy.updateRange(range);
        update();
    }

    template <typename ValuePolicy> void GenericSpinner<ValuePolicy>::setCurrentValue(const Type val)
    {
        policy.set(val);
        update();
    }

    template <typename Policy>
    typename GenericSpinner<Policy>::Type GenericSpinner<Policy>::getCurrentValue() const noexcept
    {
        return policy.get();
    }

    template <typename ValuePolicy> bool GenericSpinner<ValuePolicy>::isPreviousEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_UP)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_LEFT));
    }

    template <typename ValuePolicy> bool GenericSpinner<ValuePolicy>::isNextEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_DOWN)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_RIGHT));
    }

    template <typename ValuePolicy> bool GenericSpinner<ValuePolicy>::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (isPreviousEvent(inputEvent)) {
                stepPrevious();
                return true;
            }
            else if (isNextEvent(inputEvent)) {
                stepNext();
                return true;
            }
        }
        return false;
    }

    template <typename ValuePolicy> bool GenericSpinner<ValuePolicy>::onFocus(bool state)
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

    template <typename Policy> void GenericSpinner<Policy>::stepNext()
    {
        policy.next();
        update();
    }

    template <typename Policy> void GenericSpinner<Policy>::stepPrevious()
    {
        policy.previous();
        update();
    }

    template <typename Policy> void GenericSpinner<Policy>::update()
    {
        setText(policy.str());
    }
} // namespace gui
