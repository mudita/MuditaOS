/*
 * @file Text.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "log/log.hpp"
#include "utf8/UTF8.hpp"
#include "../core/Font.hpp"
#include "Text.hpp"


namespace gui {

Text::TextLine::TextLine( UTF8* text, uint32_t startIndex, uint32_t endIndex, Text::LineEndType endType, uint32_t pixelLength ) :
	text{text},
	startIndex{ startIndex },
	endIndex{endIndex},
	endType{ endType },
	pixelLength{ pixelLength }{

}

Text::Text() :
	Rect(),
	expandMode{ expandMode},
	textType{ textType} {

	setPenWidth( 1 );
	setPenFocusWidth( 3 );
	uint32_t fontID = FontManager::getInstance().getFontID("gt_pressura_regular_16");
	font = FontManager::getInstance().getFont(fontID);
}

Text::Text( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, ExpandMode expandMode, TextType textType, const UTF8& text) :
	Rect( parent, x, y, w, h ),
	expandMode{ expandMode},
	textType{ textType} {

	setPenWidth( 1 );
	setPenFocusWidth( 3 );
	uint32_t fontID = FontManager::getInstance().getFontID("gt_pressura_regular_16");
	font = FontManager::getInstance().getFont(fontID);

}

Text::~Text() {
}

void Text::setEditMode( EditMode mode ) {
	editMode = mode;
}

void Text::setCursorWidth( uint32_t w ) {
	cursorWidth = w;
}

void Text::setText( const UTF8& text ) {

}
void Text::clear(){

}
UTF8 Text::getText() {
	return "";
}

void Text::setFont( const UTF8& fontName) {

	uint32_t fontID = FontManager::getInstance().getFontID( fontName.c_str() );
	Font* newFont = FontManager::getInstance().getFont( fontID );
	if( newFont != nullptr ) {
		font = newFont;
//		calculateDisplayText();
	} else {
		LOG_ERROR("Font not found");
	}
}

void Text::splitTextToLines( const UTF8& text) {

	//if there are text lines erase them.
	if( !textLines.empty() ) {
		while( !textLines.empty() )
			delete textLines.front();
			textLines.pop_front();
	}

	if( text.length() == 0 )
		return;

	uint32_t index = 0;
	while( index != text.length()) {

//		TextLine* textLine =
	}
}

} /* namespace gui */
