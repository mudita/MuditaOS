/*
 * @file Text.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_WIDGETS_TEXT_HPP_
#define MODULE_GUI_GUI_WIDGETS_TEXT_HPP_

//#include <strstream>

#include "utf8/UTF8.hpp"

#include "BoxLayout.hpp"
#include "Rect.hpp"

namespace gui {

/*
 * @brief Widget that holds multiple lines of text. This widget can expand vertically if needed to hold lines of text.
 */
class Text: public Rect{
public:
	enum class LineEndType {
		EOT,     //end of source text
		BREAK,   //line had enter defined as a /r
		CONTINUE, //line was broken due to not enough space to hold all characters
		CONTINUE_SPACE //line was broken on the space character because next word doesn't fit current line.
	};

protected:
	VBox labelsBox;

	class TextLine {
	public:
		UTF8 text;
		LineEndType endType;

		TextLine( const UTF8 );
	};
public:
	Text();
	Text( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const UTF8& text = "", bool expandVertically = false);
	virtual ~Text();

	//virtual methods
	std::list<DrawCommand*> buildDrawList() override;
	void setPosition( const short& x, const short& y ) override;
	void setSize( const short& w, const short& h ) override;
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TEXT_HPP_ */
