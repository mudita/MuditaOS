/*
 * Renderer.hpp
 *
 *  Created on: 7 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_RENDERER_HPP_
#define GUI_CORE_RENDERER_HPP_

#include <vector>
#include "DrawCommand.hpp"
#include "Context.hpp"
#include "Font.hpp"

namespace gui {

enum class LineExpansionDirection {
	LINE_EXPAND_UP = 0x01,
	LINE_EXPAND_DOWN,
	LINE_EXPAND_LEFT,
	LINE_EXPAND_RIGHT
};

class Renderer {
protected:
	/**
	 * @brief Function for drawing horizontal line with given width in specified direction
	 */
	void drawHorizontalLine(
		Context* ctx, int16_t x, int16_t y,
		uint16_t width, uint16_t penWidth,
		Color color, LineExpansionDirection dir );
	/**
	 * @brief Function for drawing vertical line with given width in specified direction
	 */
	void drawVerticalLine( Context* ctx, int16_t x, int16_t y,
		uint16_t height, uint16_t penWidth,
		Color color, LineExpansionDirection dir );
	/**
	 * @brief Function that draws line on the given context
	 */
	void drawLine( Context* ctx, CommandLine* cmd );
	/**
	 * @brief Function draws rectangle on the given context;
	 */
	void drawRectangle( Context* ctx, CommandRectangle* cmd );
	/**
	 * @brief Function draws line of text with specified font on the given context;
	 */
	void drawText( Context* ctx, CommandText* cmd );
	/**
	 * @brief Draws single glyph to specified context
	 */
	void drawChar( Context* context, const int16_t x, const int16_t y,
			Font* font, FontGlyph* glyph, const Color color );
	/**
	 * @brief Draws an image with specified ID on given coordinates.
	 */
	void drawImage( Context* ctx, CommandImage* cmd );
public:
	Renderer();
	virtual ~Renderer();

	void render( Context* ctx, std::vector<DrawCommand*>& commands  );
};

} /* namespace gui */

#endif /* GUI_CORE_RENDERER_HPP_ */
