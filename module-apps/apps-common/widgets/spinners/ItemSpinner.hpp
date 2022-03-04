// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SpinnerPolicies.hpp"
#include <Item.hpp>

namespace gui
{
    template <typename Policy> class ItemSpinner : public Item
    {
      public:
        using Range = typename Policy::Range;
        using Type  = typename Policy::Type;

        using OnValueChanged = std::function<void(const Type &&)>;

        explicit ItemSpinner(Item *parent,
                             Range range,
                             Boundaries boundaries   = Boundaries::Continuous,
                             Orientation orientation = Orientation::Vertical);

        [[nodiscard]] Type getCurrentValue() const noexcept;
        void setCurrentValue(Type val);
        void setRange(Range range);

        void setFocusEdges(RectangleEdge edges);
        bool onInput(const InputEvent &inputEvent) override;
        [[nodiscard]] bool isAtMin() const;
        [[nodiscard]] bool isAtMax() const;

        OnValueChanged onValueChanged;

      private:
        void stepNext();
        void stepPrevious();
        bool isPreviousEvent(const InputEvent &inputEvent);
        bool isNextEvent(const InputEvent &inputEvent);
        void update();
        void invoke();

        Policy policy;
        RectangleEdge focusEdges = RectangleEdge::Bottom;
        Orientation orientation  = Orientation::Vertical;
        gui::Item *currentLayout = nullptr;
    };

    template <typename ValuePolicy>
    ItemSpinner<ValuePolicy>::ItemSpinner(Item *parent,
                                          ItemSpinner::Range range,
                                          Boundaries boundaries,
                                          Orientation orientation)
        : Item(), policy{range, boundaries}, orientation(orientation)
    {
        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
    }

    template <typename ValuePolicy> void ItemSpinner<ValuePolicy>::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

    template <typename Policy> void ItemSpinner<Policy>::setRange(Range range)
    {
        policy.updateRange(range);
        update();
    }

    template <typename ValuePolicy> void ItemSpinner<ValuePolicy>::setCurrentValue(const Type val)
    {
        policy.set(val);
        update();
    }

    template <typename Policy> typename ItemSpinner<Policy>::Type ItemSpinner<Policy>::getCurrentValue() const noexcept
    {
        return policy.get();
    }

    template <typename ValuePolicy> bool ItemSpinner<ValuePolicy>::isPreviousEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_DOWN)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_LEFT));
    }

    template <typename ValuePolicy> bool ItemSpinner<ValuePolicy>::isNextEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_UP)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_RIGHT));
    }

    template <typename ValuePolicy> bool ItemSpinner<ValuePolicy>::onInput(const InputEvent &inputEvent)
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

    template <typename Policy> void ItemSpinner<Policy>::stepNext()
    {
        if (policy.next()) {
            update();
            invoke();
        }
    }

    template <typename Policy> void ItemSpinner<Policy>::stepPrevious()
    {
        if (policy.previous()) {
            update();
            invoke();
        }
    }

    template <typename Policy> void ItemSpinner<Policy>::update()
    {
        if (currentLayout) {
            this->removeWidget(currentLayout);
        }
        currentLayout = policy.get();
        this->addWidget(currentLayout);
        informContentChanged();
    }
    template <typename Policy> void ItemSpinner<Policy>::invoke()
    {
        if (onValueChanged) {
            onValueChanged(getCurrentValue());
        }
    }
    template <typename Policy> bool ItemSpinner<Policy>::isAtMin() const
    {
        return policy.isAtMin();
    }
    template <typename Policy> bool ItemSpinner<Policy>::isAtMax() const
    {
        return policy.isAtMax();
    }
} // namespace gui
