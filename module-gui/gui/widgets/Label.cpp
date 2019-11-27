/*
 * Label.cpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */
#include "log/log.hpp"
#include "utf8/UTF8.hpp"

#include "../core/DrawCommand.hpp"
#include "../core/Font.hpp"

#include "Label.hpp"
#include <Style.hpp>

namespace gui {

Label::Label() :
	Rect(),
	text{ "" },
	textDisplayed{ "" },
	charDraw{0},
	stringPixelWidth{ 0 },
	textColor{ 0,0 },
	font{ nullptr },
	dotsMode{false}
{
    setFont(style::window::font::medium);
}

Label::Label( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const UTF8& newText ) :
	Rect{ parent, x, y, w, h },
	text( newText ),
	textDisplayed{ "" },
	charDraw{0},
	stringPixelWidth{ 0 },
	textColor{ 0,0 },
	dotsMode{false}
{
    setFont(style::window::font::medium);
}

Label::~Label() {
}

void Label::calculateDisplayText() {

	//calculate area needed to display text
	uint32_t availableSpace = drawArea.w;
	if( availableSpace < 0 )
		availableSpace = 0;

	//calculate number of chars that can fit available space
	uint32_t spaceConsumed;
	charDraw = font->getCharCountInSpace( text, availableSpace, spaceConsumed );
	textArea.w = (uint16_t)spaceConsumed;

	//if not all characters fit create substring
	if( charDraw < text.length())
	{
		//get as much chars as possible
		textDisplayed = text.substr( 0, charDraw );

		// if 3 dots should be placed a the end
		if( dotsMode )
		{
			//create 3 dots string
			UTF8 dotsStr = UTF8("...");

			//calculate how many dots can fit in the available space
			uint32_t dotsSpaceConsumed;
			uint32_t dotsFitting = font->getCharCountInSpace( dotsStr, drawArea.w, dotsSpaceConsumed );

			//if there is not enough space even for three dots
			if( dotsFitting < 3 )
			{
				textDisplayed = dotsStr.substr( 0, dotsFitting );
				textArea.w = (uint16_t)dotsSpaceConsumed;
				charDraw = (uint16_t)dotsFitting;
			}
			else //3 dots fit, calculate how many chars can be placed using smaller space (space for dots substracted )
			{
				availableSpace -= dotsSpaceConsumed;

				uint32_t remainingCharDraw = font->getCharCountInSpace( text, availableSpace, spaceConsumed );
				if( remainingCharDraw )
				{
					//get as much chars as possible
					textDisplayed = text.substr(0, remainingCharDraw) + dotsStr;
					textArea.w = (uint16_t)(dotsSpaceConsumed + spaceConsumed );
					charDraw = (uint16_t)textDisplayed.length();
				}
				else //only 3 dots can fit
				{
					textArea.w = (uint16_t)dotsSpaceConsumed;
					charDraw = (uint16_t)dotsFitting;
					textDisplayed = dotsStr;
				}
			}
		}
	}
	else
	{
		textDisplayed = text;
	}

	stringPixelWidth = font->getPixelWidth( textDisplayed, 0, textDisplayed.length() );
	textArea.h = font->info.line_height;

	//calculate vertical position of text
	if( alignment.isAligned(Alignment::ALIGN_VERTICAL_CENTER ))
	{
		textArea.y =
			(widgetArea.h - font->info.line_height )/2 +
			font->info.base;
	}
	else if( alignment.isAligned(Alignment::ALIGN_VERTICAL_TOP ))
	{
		textArea.y = font->info.base + margins.top;
	}
	else if( alignment.isAligned(Alignment::ALIGN_VERTICAL_BOTTOM ))
	{
		textArea.y =
			widgetArea.h -
			font->info.line_height + font->info.base -
			margins.bottom;

	}
	//calculate horizontal position o text
	if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_CENTER ))
	{
		textArea.x =( widgetArea.w - textArea.w )/2;
	}
	else if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_LEFT ))
	{
		textArea.x = margins.left;
	}
	else if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_RIGHT ))
	{
		textArea.x = widgetArea.w - textArea.w - margins.right;
	}

	//if dots mode is disabled and line mode is enabled calculate positiona and width of the line
	if( (!dotsMode) && (lineMode) && (lineFront!=nullptr) ) {
		uint32_t spaceWidth = font->getCharPixelWidth(' ');
		int32_t lineW = availableSpace - stringPixelWidth ;
		uint32_t lineY = textArea.y - font->getCharPixelHeight('a')/2;
		if( lineW < 0 ) lineW = 0;

		lineFront->setVisible(true);
		lineBack->setVisible(true);
		//both lines are visible
		if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_CENTER )) {
			lineFront->setPosition( 0, lineY );
			lineFront->setSize( lineW/2 - spaceWidth, 2 );
			lineBack->setPosition( lineW/2 + stringPixelWidth +spaceWidth, lineY);
			lineBack->setSize( lineW/2 -spaceWidth, 2 );
		}
		else if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_RIGHT)) {
			lineFront->setPosition( 0, lineY );
			lineFront->setSize( lineW-spaceWidth, 2 );
			lineBack->setVisible(false);
		}
		else if( alignment.isAligned(Alignment::ALIGN_HORIZONTAL_LEFT )) {
			lineBack->setPosition( stringPixelWidth +spaceWidth, lineY);
			lineBack->setSize( lineW-spaceWidth, 2 );
			lineFront->setVisible(false);
		}
	}
}

void Label::setText( const UTF8& text ) {
	this->text = text;
	calculateDisplayText();
}

void Label::clear() {
	this->text = UTF8("");
}

UTF8 Label::getText() {
	return text;
}

void Label::setAlignement( const Alignment& alignment ) {
	this->alignment = alignment;
	calculateDisplayText();
}

void Label::setMargins( const Margins& margins ) {
	this->margins = margins;
	calculateDisplayText();
}

void Label::setDotsMode( const bool val, const gui::Alignment dotsAlignment ) {
	dotsMode = val;
	this->dotsAlignment = dotsAlignment;
	calculateDisplayText();
}

void Label::setLineMode( const bool& val ) {
	//if line mode is disable remove the line if it was previously created
	if( val == false ) {
		if( lineFront != nullptr ) {
			this->removeWidget( lineFront );
			this->removeWidget( lineBack );
			delete lineFront;
			delete lineBack;
			lineFront = nullptr;
			lineBack = nullptr;
		}
	}
	else {
		if( lineFront == nullptr ) {
			lineFront = new Rect( this, 0,0,0,0 );
			lineBack = new Rect( this, 0,0,0,0 );
		}
	}
	calculateDisplayText();
}

std::list<DrawCommand*> Label::buildDrawList() {

	std::list<DrawCommand*> commands;

	//check if widget is visible
	if( visible == false ) {
		return commands;
	}

	//get children draw commands
	std::list<DrawCommand*> commandsChildren;
	commandsChildren = Item::buildDrawList();


	//base class draw commands
	std::list<DrawCommand*> commandsBase;
	commandsBase = gui::Rect::buildDrawList();

	//set local draw commands - text command
	CommandText* textCmd = new CommandText();
	textCmd->str = textDisplayed;
	textCmd->fontID = font->id;
	textCmd->color = textColor;

	textCmd->x = drawArea.x;
	textCmd->y = drawArea.y;
	textCmd->w = drawArea.w;
	textCmd->h = drawArea.h;
	textCmd->tx = textArea.x;
	textCmd->ty = textArea.y;
	textCmd->tw = textArea.w;
	textCmd->th = textArea.h;
	textCmd->charsWidth = stringPixelWidth;

	textCmd->areaX = widgetArea.x;
	textCmd->areaY = widgetArea.y;
	textCmd->areaW = widgetArea.w;
	textCmd->areaH = widgetArea.h;

	commands.splice( commands.end(), commandsBase );
	commands.push_back( textCmd );
	if( not commandsChildren.empty() )
		commands.splice( commands.end(), commandsChildren );

	return commands;
}

void Label::setPosition( const short& x, const short& y ) {
	gui::Rect::setPosition( x, y );
	calculateDisplayText();
}

void Label::setSize( const short& w, const short& h ) {
	gui::Rect::setSize( w, h );
	calculateDisplayText();
}

void Label::setFont( const UTF8& fontName) {

	uint32_t fontID = FontManager::getInstance().getFontID( fontName.c_str() );
	Font* newFont = FontManager::getInstance().getFont( fontID );
	if( newFont != nullptr ) {
		font = newFont;
		calculateDisplayText();
	} else {
		LOG_ERROR("Font not found!");
	}
}

void Label::setTextColor( Color color ) {
	textColor = color;
}

} /* namespace gui */
