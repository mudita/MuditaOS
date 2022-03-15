// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/text/TextFixedSize.hpp>

#include <string>
#include <type_traits>

namespace details
{
    template <typename T, typename = void> struct container_data
    {
        using value = T;
    };

    template <typename T> struct container_data<T, std::enable_if_t<not std::is_fundamental_v<T>>>
    {
        using value = typename T::value_type;
    };

    template <class T> using container_data_v = typename container_data<T>::value;

} // namespace details

namespace gui
{
    // This spinner operates on container elements and transforms the current container into a string.
    // For the containers of types that are convertible to strings like std::string, integer, floating-point integers,
    // there is no need to provide the custom formatter. However, it is possible to pass a formatter if a user wants to
    // perform custom formatting. It is not possible to use a formatter when using container of std::string.
    template <typename Container, typename Formatter = void> class StringOutputSpinner : public TextFixedSize
    {
      public:
        using range      = typename Container::range;
        using value_type = details::container_data_v<Container>;

        explicit StringOutputSpinner(Container &&container, Orientation orientation = Orientation::Vertical)
            : container{std::move(container)}, orientation{orientation}
        {
            init();
        }

        explicit StringOutputSpinner(range &range,
                                     Boundaries boundaries   = Boundaries::Fixed,
                                     Orientation orientation = Orientation::Vertical)
            : container{range, convert_boundaries(boundaries)}, orientation{orientation}
        {
            init();
        }

        explicit StringOutputSpinner(range &&range,
                                     Boundaries boundaries   = Boundaries::Fixed,
                                     Orientation orientation = Orientation::Vertical)
            : container{std::move(range), convert_boundaries(boundaries)}, orientation{orientation}
        {
            init();
        }

        using OnValueChanged = std::function<void(const value_type &)>;
        void setFocusEdges(RectangleEdge edges);
        bool onInput(const InputEvent &inputEvent) override;
        bool onFocus(bool state) override;
        [[nodiscard]] size_t size() const;
        [[nodiscard]] bool is_min() const;
        [[nodiscard]] bool is_max() const;

        OnValueChanged onValueChanged;

        void set_range(const range &range);
        void set_value(const value_type &value);
        [[nodiscard]] auto value() const noexcept;

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
        void init();
        [[nodiscard]] std::string value_as_str() const noexcept;
        void next();
        void previous();
        bool is_previous_event(const InputEvent &inputEvent);
        bool is_next_event(const InputEvent &inputEvent);
        void update();
        void invoke();

        static constexpr bool is_string =
            std::is_same_v<value_type, std::string> or std::is_convertible_v<value_type, std::string>;
        static constexpr bool is_fundamental       = std::is_fundamental_v<value_type>;
        static constexpr bool is_formatter_defined = not std::is_same_v<Formatter, void>;

        Container container;
        RectangleEdge focusEdges = RectangleEdge::Bottom;
        Orientation orientation  = Orientation::Vertical;
    };

    template <typename Container, typename Formatter>
    auto StringOutputSpinner<Container, Formatter>::value() const noexcept
    {
        return container.get();
    }

    template <typename Container, typename Formatter>
    std::string StringOutputSpinner<Container, Formatter>::value_as_str() const noexcept
    {
        if constexpr (is_string) {
            return container.get();
        }
        else if constexpr (is_fundamental and not is_formatter_defined) {
            return std::to_string(container.get());
        }
        else {
            return Formatter{}(container.get());
        }
    }

    template <typename Container, typename Formatter>
    void StringOutputSpinner<Container, Formatter>::setFocusEdges(RectangleEdge edges)
    {
        focusEdges = edges;
    }

    template <typename Container, typename Formatter>
    bool StringOutputSpinner<Container, Formatter>::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (is_previous_event(inputEvent)) {
                previous();
                return true;
            }
            else if (is_next_event(inputEvent)) {
                next();
                return true;
            }
        }
        return false;
    }

    template <typename Container, typename Formatter>
    bool StringOutputSpinner<Container, Formatter>::onFocus(bool state)
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

    template <typename Container, typename Formatter> size_t StringOutputSpinner<Container, Formatter>::size() const
    {
        return container.size();
    }

    template <typename Container, typename Formatter> bool StringOutputSpinner<Container, Formatter>::is_min() const
    {
        return container.is_min();
    }

    template <typename Container, typename Formatter> bool StringOutputSpinner<Container, Formatter>::is_max() const
    {
        return container.is_max();
    }

    template <typename Container, typename Formatter>
    void StringOutputSpinner<Container, Formatter>::set_value(const value_type &value)
    {
        container.set(value);
        update();
    }

    template <typename Container, typename Formatter>
    void StringOutputSpinner<Container, Formatter>::set_range(const range &range)
    {
        container.set_range(range);
        update();
    }

    template <typename Container, typename Formatter> void StringOutputSpinner<Container, Formatter>::next()
    {
        if (container.next()) {
            update();
            invoke();
        }
    }

    template <typename Container, typename Formatter> void StringOutputSpinner<Container, Formatter>::previous()
    {
        if (container.previous()) {
            update();
            invoke();
        }
    }
    template <typename Container, typename Formatter> void StringOutputSpinner<Container, Formatter>::update()
    {
        setText(value_as_str());
    }

    template <typename Container, typename Formatter> void StringOutputSpinner<Container, Formatter>::invoke()
    {
        if (onValueChanged) {
            onValueChanged(value());
        }
    }

    template <typename Container, typename Formatter>
    bool StringOutputSpinner<Container, Formatter>::is_previous_event(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_DOWN)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_LEFT));
    }

    template <typename Container, typename Formatter>
    bool StringOutputSpinner<Container, Formatter>::is_next_event(const InputEvent &inputEvent)
    {
        return (orientation == Orientation::Vertical && inputEvent.is(KeyCode::KEY_UP)) ||
               (orientation == Orientation::Horizontal && inputEvent.is(KeyCode::KEY_RIGHT));
    }

    template <typename Container, typename Formatter> void StringOutputSpinner<Container, Formatter>::init()
    {
        setEditMode(EditMode::Browse);
        drawUnderline(false);
        update();
    }

} // namespace gui
