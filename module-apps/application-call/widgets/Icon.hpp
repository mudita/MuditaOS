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
            constexpr uint32_t x = 9;
            constexpr uint32_t y = 58;
            constexpr uint32_t w = 82;
            constexpr uint32_t h = 20;
        } // namespace label
        namespace img
        {
            constexpr uint32_t x = 34;
            constexpr uint32_t y = 15;
            constexpr uint32_t w = 32;
            constexpr uint32_t h = 32;
        } // namespace img
    }     // namespace icon

    // Icon widget with custom image and label
    template <class T> class Icon : public Rect
    {
      public:
        typedef std::map<T, std::pair<const std::string, const std::string>> IconMap;
        Icon() = delete;
        Icon(Item *parent, const uint32_t &x, const uint32_t &y, T state, const IconMap &data)
            : Rect(parent, x, y, icon::w, icon::h), data(data)
        {
            setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
            setPenFocusWidth(style::window::default_border_focucs_w);
            setPenWidth(style::window::default_border_no_focus_w);

            img   = new gui::Image(this, icon::img::x, icon::img::y, icon::img::w, icon::img::h);
            label = new gui::Label(this, icon::label::x, icon::label::y, icon::label::w, icon::label::h);
            label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            label->setAlignement(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            label->setFont(style::window::font::verysmall);

            set(state);
        }

        virtual ~Icon()
        {
            removeWidget(img);
            delete img;
            img = nullptr;

            removeWidget(label);
            delete label;
            label = nullptr;
        }

        virtual void set(const T &state)
        {
            this->state = state;
            img->set(data.at(state).first);
            label->setText(utils::localize.get(data.at(state).second));
        }

      protected:
        Image *img   = nullptr;
        Label *label = nullptr;
        T state;
        const IconMap data;
    };
} // namespace gui
