#pragma once

#include <list>
#include <cstdint>
#include "Item.hpp"
#include "../core/Color.hpp"
#include "Style.hpp"

namespace gui
{

    class Rect : public Item
    {
      public:
        /// color used for drawing border
        Color borderColor = gui::ColorFullBlack;
        /// color for filling rectangle
        Color fillColor = Color(15, 15);
        /// number of pixels used to draw border of rectangle without focus
        uint8_t penWidth = style::window::default_border_rect_no_focus;
        /// number of pixels used to draw border of rectangle when it has focus
        uint8_t penFocusWidth = style::window::default_border_focucs_w;
        /// mark if rectangle should be filled with fill color.
        bool filled = false;
        /// mark how to draw borders
        RectangleEdgeFlags edges{RectangleEdgeFlags::GUI_RECT_ALL_EDGES};
        /// mark which edge should be flat. This will disable roundness on both sides of the edge.
        RectangleFlatFlags flatEdges = {RectangleFlatFlags::GUI_RECT_FLAT_NO_FLAT};
        /// mark if given corners should be painted (only for rounded corners)
        RectangleCornerFlags corners = {RectangleCornerFlags::GUI_RECT_ALL_CORNERS};
        /// mark if we japs should be painted. small protrusions indicating a speech bubble
        RectangleYapFlags yaps = {RectangleYapFlags::GUI_RECT_YAP_NO_YAPS};
        /// yap size in horizontal width.
        unsigned short yapSize = {style::window::messages::yaps_size_default};

      public:
        Rect();
        Rect(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);

        void setFillColor(const Color &color);
        void setBorderColor(const Color &color);
        void setPenWidth(uint8_t width);
        void setPenFocusWidth(uint8_t width);
        void setEdges(RectangleEdgeFlags edges);
        void setCorners(RectangleCornerFlags corners);
        void setFlat(RectangleFlatFlags flats);
        virtual void setYaps(RectangleYapFlags yaps);
        virtual void setYapSize(unsigned short value);
        void setFilled(bool val);
        std::list<DrawCommand *> buildDrawList() override;
    };

} /* namespace gui */
