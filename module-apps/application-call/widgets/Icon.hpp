#pragma once

#include "Image.hpp"
#include "Label.hpp"
#include <Style.hpp>
#include <log/log.hpp>
#include <i18/i18.hpp>

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
            constexpr uint32_t h = 20;
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
    template <class T> class Icon : public Rect
    {
      public:
        using IconMap = std::map<T, std::pair<const std::string, const std::string>>;
        Icon()        = delete;
        Icon(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w_margin, T state, const IconMap &data)
            : Rect(parent, x, y, icon::w + 2 * w_margin, icon::h), data(data)
        {
            setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            setPenFocusWidth(style::window::default_border_no_focus_w);
            setPenWidth(style::window::default_border_no_focus_w);
            boundingRect = new Rect(this, w_margin, 0, icon::w, icon::h);
            boundingRect->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
            boundingRect->setPenFocusWidth(style::window::default_border_focucs_w);
            boundingRect->setPenWidth(style::window::default_border_no_focus_w);

            img   = new gui::Image(boundingRect, icon::img::x, icon::img::y, 0, 0);
            label = new gui::Label(this, icon::label::x, icon::label::y, icon::label::w + 2 * w_margin, icon::label::h);
            label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            label->setAlignment(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            label->setFont(style::window::font::verysmall);

            set(state);
        }

        virtual ~Icon()
        {
  
        }

        /// @brief sets Icon state and display appropriate image and string
        /// @param state to set. @note state is also used as a key to the internal map. If the state points to the
        /// different than 1st mapping, string is displayed as bold one.
        virtual void set(const T &state)
        {
            this->state = state;
            img->set(data.at(state).first);
            label->setText(utils::localize.get(data.at(state).second));
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
        virtual bool onFocus(bool state) override
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
