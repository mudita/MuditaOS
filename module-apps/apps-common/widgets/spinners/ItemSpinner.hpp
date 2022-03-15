// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Item.hpp>
#include <InputEvent.hpp>

namespace gui
{
    template <typename Container> class ItemSpinner : public Item
    {
      public:
        using range      = typename Container::range;
        using value_type = typename Container::value_type;

        using OnValueChanged = std::function<void(const value_type &&)>;

        explicit ItemSpinner(Item *parent,
                             range range,
                             Boundaries boundaries   = Boundaries::Continuous,
                             Orientation orientation = Orientation::Vertical);

        [[nodiscard]] value_type getCurrentValue() const noexcept;
        void setCurrentValue(value_type val);
        void setRange(range range);

        void setFocusEdges(RectangleEdge edges);
        bool onInput(const InputEvent &inputEvent) override;
        [[nodiscard]] bool isAtMin() const;
        [[nodiscard]] bool isAtMax() const;

        OnValueChanged onValueChanged;

      private:
        inline typename Container::Boundaries convert_boundaries(gui::Boundaries boundaries)
        {
            switch (boundaries) {
            case gui::Boundaries::Fixed:
                return Container::Boundaries::Fixed;
            case gui::Boundaries::Continuous:
                return Container::Boundaries::Continuous;
            }
            return Container::Boundaries::Fixed;
        }
        void stepNext();
        void stepPrevious();
        bool isPreviousEvent(const InputEvent &inputEvent);
        bool isNextEvent(const InputEvent &inputEvent);
        void update();
        void invoke();

        Container container;
        RectangleEdge focusEdges = RectangleEdge::Bottom;
        Orientation orientation  = Orientation::Vertical;
        gui::Item *currentLayout = nullptr;
    };

    template <typename Container>
    ItemSpinner<Container>::ItemSpinner(Item *parent,
                                        ItemSpinner::range range,
                                        Boundaries boundaries,
                                        Orientation orientation)
        : Item(), container{range, convert_boundaries(boundaries)}, orientation(orientation)
    {
        this->parent = parent;
        if (parent != nullptr) {
            parent->addWidget(this);
        }
    }

    template <typename Container> void ItemSpinner<Container>::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

    template <typename Container> void ItemSpinner<Container>::setRange(range range)
    {
        container.updateRange(range);
        update();
    }

    template <typename Container> void ItemSpinner<Container>::setCurrentValue(const value_type val)
    {
        container.set(val);
        update();
    }

    template <typename Container>
    typename ItemSpinner<Container>::value_type ItemSpinner<Container>::getCurrentValue() const noexcept
    {
        return container.get();
    }

    template <typename Container> bool ItemSpinner<Container>::isPreviousEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_DOWN)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_LEFT));
    }

    template <typename Container> bool ItemSpinner<Container>::isNextEvent(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_UP)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_RIGHT));
    }

    template <typename Container> bool ItemSpinner<Container>::onInput(const InputEvent &inputEvent)
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

    template <typename Container> void ItemSpinner<Container>::stepNext()
    {
        if (container.next()) {
            update();
            invoke();
        }
    }

    template <typename Container> void ItemSpinner<Container>::stepPrevious()
    {
        if (container.previous()) {
            update();
            invoke();
        }
    }

    template <typename Container> void ItemSpinner<Container>::update()
    {
        if (currentLayout != nullptr) {
            this->removeWidget(currentLayout);
        }
        currentLayout = container.get();
        this->addWidget(currentLayout);
        informContentChanged();
    }
    template <typename Container> void ItemSpinner<Container>::invoke()
    {
        if (onValueChanged) {
            onValueChanged(getCurrentValue());
        }
    }
    template <typename Container> bool ItemSpinner<Container>::isAtMin() const
    {
        return container.is_min();
    }
    template <typename Container> bool ItemSpinner<Container>::isAtMax() const
    {
        return container.is_max();
    }
} // namespace gui
