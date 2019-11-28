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

#include "utf8/UTF8.hpp"

namespace gui {

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
    //show beggining or show end. dots will be on the oposite side of what to show
	bool dotsShowBeginning;
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

	virtual ~Label();
    //Label's specific methods
	virtual void setText( const UTF8& text );
    virtual void clear();
    virtual UTF8 getText();
    virtual void setAlignement( const Alignment& alignment );
    virtual void setMargins( const Margins& margins );
    void setDotsMode( const bool val , const bool showBeginning = true );
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
