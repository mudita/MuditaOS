/*
 * Label.h
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_LABEL_HPP_
#define MIDDLEWARES_GUI_WIDGETS_LABEL_HPP_

#include <string>
#include <list>

#include "../core/BoundingBox.hpp"
#include "../core/Font.hpp"
#include "../core/Color.hpp"
#include "../core/DrawCommand.hpp"
#include "../Common.hpp"
#include "Margins.hpp"
#include "Alignment.hpp"
#include "Rect.hpp"

#include "Style.hpp"
#include "utf8/UTF8.hpp"

namespace gui {

    namespace meta
    {
        struct Meta
        {
        };

        struct Item : public Meta
        {
            uint32_t radius = 0;
        };

        struct Rect : public Item
        {
            Rect &operator()(std::array<uint32_t, 4> xywh)
            {
                x = xywh[0], y = xywh[1], w = xywh[2], h = xywh[3];
                return *this;
            }
            uint32_t x = 0, y = 0, w = 0, h = 0;
            int32_t no_focus = style::window::default_border_no_focus_w;
            int32_t focus = style::window::default_border_no_focus_w;
            std::string font = style::window::font::medium;
            bool dots = true;
            uint32_t align = gui::Alignment::ALIGN_HORIZONTAL_LEFT | gui::Alignment::ALIGN_VERTICAL_CENTER;
            RectangleEdgeFlags edges = gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP | gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM;
        };

        struct Label : public Rect
        {
            const UTF8 text;
            Margins margins;
            Label(std::array<uint32_t, 4> xywh)
            {
                operator()(xywh);
            }
        };
    }; // namespace meta

class Label: public Rect {
protected:
	UTF8 text;
	UTF8 textDisplayed;
	uint32_t charDrawableCount;
	//number of pixels ccupied by displayed text;
	uint32_t stringPixelWidth;

	//color of the text
	Color textColor;
	//font pointer
	Font* font;
	//margins for text
	Margins margins;
    //type of alignment
	Alignment alignment;
    //flag that defines if 3 dots (ellipsis) are to be displayed at the end of label's text.
	bool dotsMode;
    //truncate a beginning or an end.
	bool dotsTruncateEnd;
    //flag that defines if a remaining area of the label has a horizontal line.
	bool lineMode;

	//area specified in pixels occupied by text inside label space.
	//This defines also position of the text considering alignment and margins.
	BoundingBox textArea;
    //widgets to add line
	Rect* lineFront = nullptr;

	Rect* lineBack = nullptr;
    void calculateDisplayText();

public:
    Label();
    Label( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const UTF8& text = UTF8{} );
    Label(Item *parent, meta::Label label);

    virtual ~Label();
    //Label's specific methods
	virtual void setText( const UTF8& text );
    virtual void clear();
    virtual UTF8 getText();
    virtual void setAlignement( const Alignment& alignment );
    virtual void setMargins( const Margins& margins );
    void setDotsMode( const bool val , const bool truncateEnd = true );
    /**
	 * @brief Defines if remaining area of the label has a horizontal line.
	 */
	void setLineMode( const bool& val );
    void setTextColor( Color color );

	void setFont( const UTF8& fontName );
    //virtual methods
	std::list<DrawCommand*> buildDrawList() override;
    void setPosition( const short& x, const short& y ) override;

	void setSize( const short& w, const short& h ) override;
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_LABEL_HPP_ */
