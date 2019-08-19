/*
 * @file Text.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <iterator>
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

	//insert first empty text line
	textLines.push_back( new TextLine( UTF8(""), 0, 0, LineEndType::EOT, 0 ) );
	firstLine = textLines.begin();
	lastLine = textLines.begin();
	setBorderColor( gui::ColorFullBlack );
	setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
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

	//insert first empty text line
	textLines.push_back( new TextLine( UTF8(""), 0, 0, LineEndType::EOT, 0 ) );
	firstLine = textLines.begin();
	lastLine = textLines.begin();
	setBorderColor( gui::ColorFullBlack );
	setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
}

Text::~Text() {
	clear();
}

void Text::setEditMode( EditMode mode ) {
	editMode = mode;
}

void Text::setCursorWidth( uint32_t w ) {
	cursorWidth = w;
}

void Text::setText( const UTF8& text ) {
	clear();
	//erase default empty line
	delete textLines.front();
	textLines.pop_front();
	textLines.clear();
	//split and add new lines
	splitTextToLines(text);
	recalculateDrawParams();
}
void Text::clear(){
	//if there are text lines erase them.
	if( !textLines.empty() ) {
		while( !textLines.empty() ) {
			delete textLines.front();
			textLines.pop_front();
		}
	}
	textLines.clear();
	//insert first empty text line
	textLines.push_back( new TextLine( UTF8(""), 0, 0, LineEndType::EOT, 0 ) );
	firstLine = textLines.begin();
	lastLine = textLines.begin();
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
	recalculateDrawParams();
}

void Text::splitTextToLines( const UTF8& text) {

	if( text.length() == 0 )
		return;

	//copy provided text to internal buffer
	uint32_t index = 0;
	uint32_t totalLength = text.length();

	uint32_t availableSpace = getAvailableHPixelSpace();

	while( index < totalLength ) {

		UTF8 textCopy = text.substr(index,totalLength-index);
		//find how many character fit in the widget's width
		//this doesnt include any line breaking conditinos like enter or space because line is too long
		uint32_t spaceConsumed = 0;
		uint32_t charCount = font->getCharCountInSpace( textCopy, availableSpace, spaceConsumed );
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

	firstLine = textLines.begin();
	lastLine = textLines.begin();
//	for( TextLine* tl : textLines )
//		LOG_INFO("Text Input Line: [%s]", tl->text.c_str());
}

void Text::reworkLines( std::list<TextLine*>::iterator it ) {

}


std::list<DrawCommand*> Text::buildDrawList() {
	return Rect::buildDrawList();
}
void Text::setPosition( const short& x, const short& y ) {
	Rect::setPosition(x, y);
	recalculateDrawParams();
}

void Text::setSize( const short& w, const short& h ) {
	Rect::setSize( w, h );
	recalculateDrawParams();
}

bool Text::onInput( const InputEvent& inputEvent ) {

	//process only short release events
	if( inputEvent.state != InputEvent::State::keyReleasedShort ) {
		return false;
	}

	//if char is a new line char then create new line and move caret and return
	if( inputEvent.keyChar == 0x0A) {
		if( textType == TextType::MULTI_LINE )
			return handleEnter();
	}
	//backspace handling
	else if( inputEvent.keyChar  == 0x08 ) {
		return handleBackspace();
	}
	else { //normal char -> add and check pixel width
		return handleChar( inputEvent );
	}

	return false;
}

bool Text::onActivated( void* data ) {
	return false;
}

bool Text::onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) {
	return false;
}

bool Text::moveCursor( const MoveDirection& direction ) {

	//if cursor is standing on the first line return false to allow focus change to previous widget
	if( firstLine == textLines.begin()) {

		//TODO add check for bariers preventing navigation to other widgets.

		return false;
	}

	if( direction == MoveDirection::MOVE_LEFT ) {

	}
	else if( direction == MoveDirection::MOVE_RIGHT ) {

	}
	else if( direction == MoveDirection::MOVE_DOWN ) {

	}
	else if( direction == MoveDirection::MOVE_UP ) {
		--firstLine;
	}


	return false;
}

bool Text::handleBrowsing( const InputEvent& inputEvent ) {
	switch( inputEvent.keyCode )
	{
		case (KeyCode::KEY_UP):
		{
			return moveCursor( MoveDirection::MOVE_UP );
		} break;
		case KeyCode::KEY_DOWN:
		{
			return moveCursor( MoveDirection::MOVE_DOWN );
		} break;
		default:
		{
			LOG_ERROR("Received unknown navigation key");
		}
	};
	return false;
}

bool Text::handleNavigation( const InputEvent& inputEvent ) {
	switch( inputEvent.keyCode )
	{
		case (KeyCode::KEY_UP):
		{
			return moveCursor( MoveDirection::MOVE_UP );
		} break;
		case KeyCode::KEY_DOWN:
		{
			return moveCursor( MoveDirection::MOVE_DOWN );
		} break;
		case KeyCode::KEY_LEFT:
		{
			return moveCursor( MoveDirection::MOVE_LEFT );
		} break;
		case KeyCode::KEY_RIGHT:
		{
			return moveCursor( MoveDirection::MOVE_RIGHT );
		} break;
		default:
		{
			LOG_ERROR("Received unknown navigation key");
		}
	};
	return false;
}

bool Text::handleEnter() {
	return true;
}

bool Text::handleBackspace() {
	return true;
}

bool Text::handleChar( const InputEvent& inputEvent ) {

	//get text line where cursor is standing
	TextLine* currentTextLine = getCursorTextLine();

	//calculate width of the character that is going to be inserted
	uint32_t charWidth = font->getCharPixelWidth( inputEvent.keyChar );

	//insert character into string in currently selected line
	if( currentTextLine->text.insertCode( inputEvent.keyChar, cursorColumn ) == false )
		return false;

	//if sum of the old string and width of the new character are greater than available space run lines rework procedure
	uint32_t linesCount = textLines.size();
	uint32_t availableSpace = getAvailableHPixelSpace();
	uint32_t currentWidth = currentTextLine->pixelLength;
	if( currentWidth + charWidth > availableSpace ) {
		auto it = firstLine;
		std::advance(it, cursorRow );
		reworkLines( it );

		//change cursor position
	}
	//no line splitting, update pixel width and proceed
	else {
		currentTextLine->pixelLength = font->getPixelWidth( currentTextLine->text );
		++cursorColumn;
	}

	//if number of text lines have increased, text widget is multi-line and expandable change widgets space
	if( linesCount != textLines.size()) {

	}

	recalculateDrawParams();
	//calculate new position of the cursor

	return true;
}

Text::TextLine* Text::getCursorTextLine() {
	auto it = firstLine;
	//TODO add check for distance to advance
	std::advance(it, cursorRow );
	return *it;
}

void Text::recalculateDrawParams() {

	//calculate number of lines for displaying text
	int32_t h = widgetArea.h - margins.top - margins.bottom - 2*radius;
	if( h < 0 )
		h = 0;

	//remove all old labels
	for( uint32_t i=0; i<labelLines.size(); ++i ) {
		removeWidget( labelLines[i]);
		delete labelLines[i];
	}

	labelLines.clear();

	//calculate how many rows can fit in available height.
	uint32_t rowCount = h / font->info.line_height;
	rowCount = (rowCount == 0)?1:rowCount;

	if( textType == TextType::SINGLE_LINE ) {
		rowCount = 1;
	}

	//if there is not enough space for single line start from 0 and ignore vertical margins
	uint32_t startY = ( h < font->info.line_height )?0:margins.top;

	//create labels to display text. There will be always at least one.
	for( uint32_t i=0; i<rowCount; i++ ) {
		gui::Label* label = new gui::Label( this, margins.left, startY, widgetArea.w - margins.left - margins.right, font->info.line_height );
		label->setFilled( false );
		label->setFont( font-> getName() );
		label->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
		label->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
		labelLines.push_back( label );
		startY += font->info.line_height;
	}

	//assign text to all lines
	auto textIterator = firstLine;
	for( uint32_t i=0; i<labelLines.size(); i++ ) {
		if( textIterator == textLines.end())
			break;
		labelLines[i]->setText( (*textIterator)->text );
		lastLine = textIterator;
		textIterator++;

	}
}

void Text::setMargins( const Margins& margins ) {
	this->margins = margins;
	recalculateDrawParams();
}

} /* namespace gui */
