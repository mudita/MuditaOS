// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        Color borderColor = Color(0, 0);
        /// color for filling rectangle
        Color fillColor = Color(15, 15);
        /// number of pixels used to draw border of rectangle without focus
        uint8_t penWidth = style::window::default_border_rect_no_focus;
        /// number of pixels used to draw border of rectangle when it has focus
        uint8_t penFocusWidth = style::window::default_border_focus_w;
        /// mark if rectangle should be filled with fill color.
        bool filled = false;
        /// mark how to draw borders
        RectangleEdge edges{RectangleEdge::All};
        /// mark which edge should be flat. This will disable roundness on both sides of the edge.
        RectangleFlatEdge flatEdges = {RectangleFlatEdge::None};
        /// mark if given corners should be painted (only for rounded corners)
        RectangleRoundedCorner corners = {RectangleRoundedCorner::All};
        /// mark if we japs should be painted. small protrusions indicating a speech bubble
        RectangleYap yaps = {RectangleYap::None};
        /// yap size in horizontal width.
        unsigned short yapSize = style::window::default_rect_yaps;

      public:
        Rect();
        Rect(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h);

        void setFillColor(const Color &color);
        void setBorderColor(const Color &color);
        void setPenWidth(uint8_t width);
        void setPenFocusWidth(uint8_t width);
        void setEdges(RectangleEdge edges);
        void setCorners(RectangleRoundedCorner corners);
        void setFlat(RectangleFlatEdge flats);
        virtual void setYaps(RectangleYap yaps);
        virtual void setYapSize(unsigned short value);
        void setFilled(bool val);
        void buildDrawListImplementation(std::list<Command> &commands) override;

        void accept(GuiVisitor &visitor) override;
    };

} /* namespace gui */
