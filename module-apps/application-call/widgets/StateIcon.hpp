// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <i18n/i18n.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <log/log.hpp>
#include <Style.hpp>

#include <map>

namespace gui
{
    namespace icon
    {
        constexpr uint32_t w = 100;
        constexpr uint32_t h = 100;

        namespace label
        {
            constexpr uint32_t x = 0;
            constexpr uint32_t y = 58;
            constexpr uint32_t w = icon::w;
            constexpr uint32_t h = 22;
        } // namespace label
        namespace img
        {
            constexpr uint32_t x = 34;
            constexpr uint32_t y = 15;
        } // namespace img
    }     // namespace icon

    /// @brief Icon widget with custom predefined images and strings
    /// images and strings are stored in predefined map passed to the class during construction
    /// class T is used as on object state as well as a key to the map.
    ///
    /// @note Desired visible area is icon::w x icon::h. However, the whole widget area might be set to bigger one w x
    /// icon::h where w = icon::w + 2 * w_margin. It is necessary as it is possible that text will exceed Icon visible
    /// area

    template <class T> class StateIcon : public Rect
    {
      public:
        using IconMap = std::map<T, std::pair<const std::string, const std::string>>;
        StateIcon()   = delete;
        StateIcon(
            Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w_margin, T state, const IconMap &data)
            : Rect(parent, x, y, icon::w + 2 * w_margin, icon::h), data(data)
        {
            setEdges(RectangleEdge::None);
            setPenFocusWidth(style::window::default_border_no_focus_w);
            setPenWidth(style::window::default_border_no_focus_w);
            boundingRect = new Rect(this, w_margin, 0, icon::w, icon::h);
            boundingRect->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
            boundingRect->setPenFocusWidth(style::window::default_border_focus_w);
            boundingRect->setPenWidth(style::window::default_border_no_focus_w);

            img   = new gui::Image(boundingRect, icon::img::x, icon::img::y, 0, 0);
            label = new gui::Label(this, icon::label::x, icon::label::y, icon::label::w + 2 * w_margin, icon::label::h);
            label->setEdges(RectangleEdge::None);
            label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            label->setFont(style::window::font::verysmall);

            set(state);
        }

        virtual ~StateIcon()
        {}

        /// @brief sets Icon state and display appropriate image and string
        /// @param state to set. @note state is also used as a key to the internal map. If the state points to the
        /// different than 1st mapping, string is displayed as bold one.
        virtual void set(const T &state)
        {
            this->state = state;
            img->set(data.at(state).first);
            label->setText(utils::translate(data.at(state).second));
            using namespace style::window;
            label->setFont(data.find(state) != data.begin() ? font::verysmallbold : font::verysmall);
        }

        T get() const
        {
            return state;
        }

        /// @brief sets Icon state to the next one (from the map) and displays next predfined image and string
        void setNext()
        {
            auto it = data.find(state);
            it++;
            if (it != data.end()) {
                set(it->first);
            }
            else {
                set(data.begin()->first);
            }
        }

        // sets/clears focus of internal boundingrect
        bool onFocus(bool state) override
        {
            Item::onFocus(state);
            setFocusItem(state ? boundingRect : nullptr);

            return true;
        }

      protected:
        Image *img         = nullptr;
        Label *label       = nullptr;
        Rect *boundingRect = nullptr;
        T state;
        const IconMap data; // internal map with predefined sets of images and strings to be displayed
    };
} // namespace gui
