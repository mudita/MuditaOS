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

Text::TextLine::TextLine( const UTF8& text, uint32_t startIndex, uint32_t endIndex, Text::LineEndType endType, uint32_t pixelLength ) :
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

Text::Text( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h,
		const UTF8& text, ExpandMode expandMode, TextType textType) :
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
	splitTextToLines(text);
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

	//copy provided text to internal buffer
	uint32_t index = 0;
	uint32_t totalLength = text.length();

	while( index < totalLength ) {

		UTF8 textCopy = text.substr(index,totalLength-index);
		//find how many character fit in the widget's width
		//this doesnt include any line breaking conditinos like enter or space because line is too long
		uint32_t spaceConsumed = 0;
		uint32_t charCount = font->getCharCountInSpace( textCopy, widgetArea.w, spaceConsumed );
		UTF8 tmpText = textCopy.substr( 0, charCount );

		//some default values
		uint32_t startIndex = 0;
		uint32_t endIndex = totalLength;
		LineEndType lineEndType = LineEndType::EOT;

		//check if this is not the end of the text
		if( index + charCount == totalLength ) {
			//try to find first enter.
			uint32_t enterIndex = tmpText.find( "\n",0);
			if( enterIndex != UTF8::npos ) {
				endIndex = index+enterIndex;
				index += enterIndex + 1;
				lineEndType = LineEndType::BREAK;
				textLines.push_back( new TextLine( tmpText.substr(0,enterIndex), startIndex, endIndex, lineEndType, spaceConsumed ) );
				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
			} //no enter found last line can be copied as a whole.
			else {
				startIndex = index;
				endIndex = totalLength;
				textLines.push_back( new TextLine( tmpText, startIndex, endIndex, lineEndType, spaceConsumed ) );
				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
				index += charCount;
			}
		}
		//if it wasn't the last line search for enter or space and break the line on it.
		else {

			startIndex = index;

			//try to find first enter.
			uint32_t enterIndex = tmpText.find( "\n",0);
			if( enterIndex != UTF8::npos ) {
				endIndex = index+enterIndex;
				index += enterIndex + 1;
				lineEndType = LineEndType::BREAK;
				textLines.push_back( new TextLine( tmpText.substr(0,enterIndex), startIndex, endIndex, lineEndType, spaceConsumed ) );
				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
			}
			else {
				//if there was no enter look for last space in the tmpText and break line on it
				uint32_t spaceIndex = tmpText.findLast( " ",tmpText.length()-1);

				//if there was no space take as many characters as possible and add CONTINUE ending
				if( spaceIndex == UTF8::npos ) {
					endIndex = index+charCount;
					index += charCount;
					lineEndType = LineEndType::CONTINUE;
					textLines.push_back( new TextLine( tmpText, startIndex, endIndex, lineEndType, spaceConsumed ) );
					LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
				}
				else {
					lineEndType = LineEndType::CONTINUE_SPACE;

					uint32_t spaceWidth = font->getPixelWidth(" ",0,1);
					//if space is last character in string erase it and add appropriate CONTINUE_SPACE ending
					if( spaceIndex == tmpText.length()-1 ) {
						endIndex = index+charCount-1;
						index += charCount;
						textLines.push_back( new TextLine( tmpText.substr(0,tmpText.length()-1),
							startIndex, endIndex, lineEndType, spaceConsumed - spaceWidth) );
						LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
					}
					else {
						endIndex = index+spaceIndex;
						index += spaceIndex+1;
						textLines.push_back( new TextLine( tmpText.substr(0,spaceIndex),
							startIndex, endIndex, lineEndType, spaceConsumed - spaceWidth) );
						LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
					}
				}
			}
		}
	}
//	for( TextLine* tl : textLines )
//		LOG_INFO("Text Input Line: [%s]", tl->text.c_str());
}

std::list<DrawCommand*> Text::buildDrawList() {
	return Item::buildDrawList();
}
void Text::setPosition( const short& x, const short& y ) {

}

void Text::setSize( const short& w, const short& h ) {

}

} /* namespace gui */
