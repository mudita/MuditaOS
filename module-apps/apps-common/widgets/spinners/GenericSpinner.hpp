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

        explicit GenericSpinner(Range range, Boundaries boundaries = Boundaries::Continuous);

        void setFocusEdges(RectangleEdge edges);

        [[nodiscard]] Type getCurrentValue() const noexcept;

        void setCurrentValue(Type val);

      private:
        bool onInput(const InputEvent &inputEvent) override;

        bool onFocus(bool state) override;

        void stepNext();

        void stepPrevious();

        void update();

        Policy policy;
        RectangleEdge focusEdges = RectangleEdge::Bottom;
    };

    template <typename ValuePolicy>
    GenericSpinner<ValuePolicy>::GenericSpinner(GenericSpinner::Range range, Boundaries boundaries)
        : policy{range, boundaries}
    {
        setEditMode(EditMode::Browse);
        drawUnderline(false);
        update();
    }

    template <typename ValuePolicy> void GenericSpinner<ValuePolicy>::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

    template <typename ValuePolicy> void GenericSpinner<ValuePolicy>::setCurrentValue(const Type val)
    {
        policy.set(val);
        update();
    }

    template <typename ValuePolicy> bool GenericSpinner<ValuePolicy>::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case KeyCode::KEY_UP:
                stepNext();
                return true;
            case KeyCode::KEY_DOWN:
                stepPrevious();
                return true;
            default:
                break;
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

    template <typename Policy>
    typename GenericSpinner<Policy>::Type GenericSpinner<Policy>::getCurrentValue() const noexcept
    {
        return policy.get();
    }

} // namespace gui
