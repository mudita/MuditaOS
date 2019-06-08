
/*
 * Renderer.cpp
 *
 *  Created on: 7 maj 2019
 *      Author: robert
 */

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

//for memset
#include <string.h>
#include "Color.hpp"
#include "Renderer.hpp"
#include "Context.hpp"
#include "Font.hpp"
#include "ImageManager.hpp"
#include "../Common.hpp"
//utils
#include "log/log.hpp"

namespace gui {

Renderer::Renderer() {
	// TODO Auto-generated constructor stub

}

Renderer::~Renderer() {
	// TODO Auto-generated destructor stub
}

void Renderer::drawLine( Context* ctx, CommandLine* cmd ) {

}

//any checks are not performed as this should be done by function handling draw command
void Renderer::drawHorizontalLine( Context* ctx, int16_t x, int16_t y,
	uint16_t width, uint16_t penWidth,
	Color color, LineExpansionDirection dir ) {

	//if color is fully transparent - return
	if( color.alpha == 0x0F )
		return;

	uint32_t drawOffset = y*ctx->getW() + x;
	int32_t rowStride;

	//line can be expanded up or down, any other option will cause function to return
	if( dir == LineExpansionDirection::LINE_EXPAND_DOWN )
		rowStride = ctx->getW();
	else if( dir == LineExpansionDirection::LINE_EXPAND_UP )
		rowStride = -ctx->getW();
	else
		return;

	for( uint32_t i = 0; i<penWidth; i++ ) {
		memset( ctx->getData() + drawOffset, color.intensivity, width );
		drawOffset += rowStride;
	}
}

//any checks are not performed as this should be done by function handling draw command
void Renderer::drawVerticalLine( Context* ctx, int16_t x, int16_t y,
	uint16_t height, uint16_t penWidth,
	Color color, LineExpansionDirection dir ) {

	//if color is fully transparent - return
	if( color.alpha == 0x0F )
		return;

	uint32_t drawOffset = y*ctx->getW() + x;
	if( dir == LineExpansionDirection::LINE_EXPAND_RIGHT ) {
		//no action needed unless there is need to draw gradient
	}
	else if( dir == LineExpansionDirection::LINE_EXPAND_LEFT ) {
		drawOffset -= penWidth;
	}
	else {
		return;
	}

	int32_t rowStride = ctx->getW();

	for( uint32_t i = 0; i<height; i++ ) {
		memset( ctx->getData() + drawOffset, color.intensivity, penWidth );
		drawOffset += rowStride;
	}
}

void Renderer::drawRectangle( Context* ctx, CommandRectangle* cmd ) {

	//check if there is anything to draw
	if( cmd->w == 0 || cmd->h == 0 ){
		return;
	}

	if(
		((cmd->fillColor.alpha == 0x0F) && (cmd->borderColor.alpha == 0x0F)) ||
		((cmd->filled == false ) && (cmd->borderColor.alpha == 0x0F))
	) {
		return;
	}

	//get copy of original context using x,y of draw coordinates and original zie of the widget

	Context* drawCtx;
	bool copyContext = false;
	int16_t wgtX = 0,wgtY = 0;
	uint16_t wgtW = cmd->areaW, wgtH=cmd->areaH;
	//check if there is a need or making copy of context to use is as background
	if( (cmd->areaW == cmd->w) && (cmd->areaH == cmd->h)) {
		drawCtx = ctx;
		wgtX = cmd->x;
		wgtY = cmd->y;
	}
	else {
		copyContext = true;
		drawCtx= ctx->get( cmd->x, cmd->y, cmd->areaW, cmd->areaH );
	}

	//if rounding of corners is 0
	if( cmd->radius == 0 ) {
		//fill field inside the rectangle if fill flag is set
		if( cmd->filled ) {
			uint32_t offset = wgtY*drawCtx->getW() + wgtX;
			for( int32_t y=0; y<cmd->areaH;y++) {
				memset( drawCtx->getData() + offset, cmd->fillColor.intensivity, wgtW );
				offset += drawCtx->getW();
			}
		}
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP )
			drawHorizontalLine( drawCtx, wgtX, wgtY, wgtW, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM )
			drawHorizontalLine( drawCtx, wgtX, wgtY + cmd->areaH-1, wgtW, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_UP );
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT )
			drawVerticalLine( drawCtx, wgtX, wgtY, wgtH, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_RIGHT );
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT )
			drawVerticalLine( drawCtx, wgtX + cmd->areaW, wgtY, wgtH, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_LEFT );
	}
	else {

		//calculate centers of circle for all corners
		int16_t xcTopRight = wgtX+cmd->w - cmd->radius;
		int16_t xcTopLeft = wgtX+cmd->radius;
		int16_t xcBottomRight = wgtX+xcTopRight;
		int16_t xcBottomLeft = wgtX+xcTopLeft;

		int16_t ycTopRight = wgtY+cmd->radius+1;
		int16_t ycTopLeft = wgtY+cmd->radius+1;
		int16_t ycBottomRight = wgtY+cmd->h - cmd->radius -1;
		int16_t ycBottomLeft = wgtY+cmd->h - cmd->radius -1;

		int x = cmd->radius, y = 0;

		//top right corner
		if( (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT) &&
			!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT))	{
			drawHorizontalLine( drawCtx, wgtX + xcTopRight + x - cmd->penWidth, wgtY + ycTopRight - y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
			drawVerticalLine( drawCtx, wgtX +xcTopRight + y, wgtY + ycTopRight - x, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
		}

		//bottom right corner
		if( (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_RIGHT) &&
			!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT))	{
			drawHorizontalLine( drawCtx, wgtX + xcBottomRight + x - cmd->penWidth, wgtY + ycBottomRight + y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
			drawVerticalLine( drawCtx, wgtX + xcBottomRight - y, wgtY + ycBottomRight + x - cmd->penWidth + 1, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
		}

		//upper left corner
		if( (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_LEFT) &&
			!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT))	{
			drawHorizontalLine( drawCtx, wgtX + xcTopLeft - x, wgtY + ycTopLeft - y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
			drawVerticalLine( drawCtx, wgtX + xcTopLeft + y + 1, wgtY + ycTopLeft - x, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
		}

		//Lower left corner
		if( (cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_LEFT) &&
			!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT))	{
			drawHorizontalLine( drawCtx, wgtX + xcBottomLeft - x, wgtY + ycBottomLeft - y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
			drawVerticalLine( drawCtx, wgtX + xcBottomLeft + y + 1, wgtY + ycBottomLeft + x + 1 - cmd->penWidth, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
		}

		// Initialising the value of P
		int P = 1 - cmd->radius;
		while (x > y)
		{
			y++;

			// Mid-point is inside or on the perimeter
			if (P <= 0)
				P = P + 2*y + 1;

			// Mid-point is outside the perimeter
			else
			{
				x--;
				P = P + 2*y - 2*x + 1;
			}

			// All the perimeter points have already been printed
			if (x < y)
				break;

			//upper right corner
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT) {

				//corner is sharp
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {

				}
				else { //classic rounded corner
					drawHorizontalLine( drawCtx, wgtX+xcTopRight + x - cmd->penWidth, wgtY+ycTopRight - y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
					if (x != y)
						drawVerticalLine( drawCtx, wgtX+xcTopRight + y, wgtY+ycTopRight - x, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}

			//bottom right corner
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_RIGHT) {
				//corner is sharp
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT ) {

				}
				else { //classic rounded corner
					drawHorizontalLine( drawCtx, wgtX+xcBottomRight + x - cmd->penWidth, wgtY+ycBottomRight + y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
					if (x != y)
						drawVerticalLine( drawCtx, wgtX+xcBottomRight + y, wgtY+ycBottomRight + x - cmd->penWidth + 1, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}

			//upper left corner
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_LEFT) {
				//corner is sharp
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT) {
					void drawText( Context* ctx, CommandText* cmd );
				}
				else { //classic rounded corner
					drawHorizontalLine( drawCtx, wgtX+xcTopLeft - x, wgtY+ycTopLeft - y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
					if (x != y)
						drawVerticalLine( drawCtx, wgtX+xcTopLeft - y + 1, wgtY+ycTopLeft - x, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}

			//lower left corner
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_LEFT) {
				//corner is sharp
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT) {

				}
				else { //classic rounded corner
					drawHorizontalLine( drawCtx, wgtX+xcBottomLeft - x, wgtY+ycBottomLeft + y, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
					if (x != y)
						drawVerticalLine( drawCtx, wgtX+xcBottomLeft - y + 1, wgtY+ycBottomLeft + x - cmd->penWidth + 1, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}
		}

		//render edges between corners
		int16_t xe, ye,le;
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP ) {
			xe = wgtX+cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT));
			ye = wgtY;
			le = wgtW - xe - cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT));
			drawHorizontalLine( drawCtx, xe, ye, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		}
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM ) {
			xe = wgtX+cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT));
			ye = wgtH-1;
			le = wgtW - xe - cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT));
			drawHorizontalLine( drawCtx, xe, ye, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_UP );
		}

		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT ) {
			xe = 0;
			ye = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT ));
			le = wgtH - ye - cmd->radius*(!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT));
			drawVerticalLine( drawCtx, xe, ye, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_RIGHT );
		}
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT ) {
			xe = cmd->areaW;
			ye = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ));
			le = wgtH - ye - cmd->radius*(!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT));
			drawVerticalLine( drawCtx, xe, ye, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_LEFT );
		}
	}

	//if drawing was performed in temporary context
	if( copyContext) {
		//reinsert drawCtx into bast context
		ctx->insert( cmd->x, cmd->y, drawCtx );
		//remove draw context
		delete drawCtx;
	}

}

void Renderer::drawChar( Context* context, const int16_t x, const int16_t y,
		Font* font, FontGlyph* glyph, const Color color ) {

	uint8_t* drawPtr = context->getData() + x + (y-glyph->yoffset)*context->getW();
	uint8_t* glyphPtr = (uint8_t*)glyph->data;

	for( uint16_t yy = 0; yy < glyph->height; ++yy )
	{
		for( uint16_t xx = 0; xx<glyph->width; ++xx )
		{
			if( *(glyphPtr + xx) == 0 )
				*(drawPtr + xx) = 0x0F & color.intensivity;
		}
		drawPtr += context->getW();
		glyphPtr += glyph->width;
	}

}

void Renderer::drawText( Context* ctx, CommandText* cmd ) {

	//check if there are any characters to draw in the string provided with message.
	if( cmd->str.length() == 0 ) {
		return;
	}

//	Context* drawCtx;
//	bool copyContext = false;
//	//check if there is a need or making copy of context to use is as background
//	if( (cmd->areaW == cmd->w) && (cmd->areaH == cmd->h)) {
//		drawCtx = ctx;
//	}
//	//get copy of original context using x,y of draw coordinates and original size of the widget
//	else {
//		copyContext = true;
//		drawCtx= ctx->get( cmd->x, cmd->y, cmd->areaW, cmd->areaH );
//	}

	Context* drawCtx = ctx->get( cmd->x, cmd->y, cmd->areaW, cmd->areaH );

	//retrieve font used to draw text
	FontManager& fontManager = FontManager::getInstance();
	Font* font = fontManager.getFont( cmd->fontID );

	int16_t posX = cmd->tx;
	int16_t posY = cmd->ty;

	//draw every sign
	uint32_t idLast = 0, idCurrent = 0;
	for( uint32_t i=0; i<cmd->str.length(); ++i )
	{
		if( i == 0 )
		{
			idCurrent = cmd->str[i];
			FontGlyph* glyph = font->glyphs.find(idCurrent)->second;

			if( glyph != NULL)
			{
				drawChar( drawCtx, posX + glyph->xoffset, posY, font, glyph, cmd->color );
				posX += glyph->xadvance ;//- glyph->xoffset;
			}
		}
		else
		{
			idCurrent = cmd->str[i];
			FontGlyph* glyph = font->glyphs.find(idCurrent)->second;

			int32_t kernValue = font->getKerning( idLast, idCurrent );

			if( glyph != NULL)
			{
//#ifdef BUILD_UNIT_TESTS
				if(( posX + glyph->xoffset>=drawCtx->getW() ) || (posX + glyph->xoffset < 0)) {
					LOG_FATAL( "Drawing outside context's X boundary for glyph: %d", glyph->id);
					return;
				}
				if(( posY >= drawCtx->getH() ) || (posY < 0)) {
					LOG_FATAL( "Drawing outside context's Y boundary for glyph: %d", glyph->id);
					return;
				}
//#endif
				drawChar( drawCtx, posX + glyph->xoffset + kernValue, posY, font, glyph, cmd->color );
				posX += glyph->xadvance+kernValue;

			}
		}
		idLast = idCurrent;
	}

	//reinsert drawCtx into bast context
	ctx->insert( cmd->x, cmd->y, drawCtx );

	//remove draw context
//	if( copyContext )
		delete drawCtx;
}

void Renderer::drawImage( Context* ctx, CommandImage* cmd ) {

	//retrive pixmap from the pixmap manager
	ImageMap* imageMap = ImageManager::getInstance().getImageMap( cmd->imageID );

	//if image is not found return;
	if( imageMap == nullptr )
		return;

	//get copy of original context using x,y of draw coordinates and original size of the widget
	Context* drawCtx = ctx->get( cmd->x, cmd->y, cmd->areaW, cmd->areaH );

	if( imageMap->getType() == gui::ImageMap::Type::PIXMAP ) {
		PixMap* pixMap = reinterpret_cast<PixMap*>(imageMap);
		//draw image on temporeary context
		uint32_t offsetImage = 0;
		uint32_t offsetContext = 0;
		uint32_t imageWidth = pixMap->getWidth();
		uint32_t contextWidth = drawCtx->getW();
		uint8_t* ctxData = drawCtx->getData();
		uint8_t* pixData = pixMap->getData();
		for( uint32_t i=0; i<pixMap->getHeight(); i++ ) {
			memcpy( ctxData + offsetContext, pixData + offsetImage, imageWidth );
			offsetImage += imageWidth;
			offsetContext += contextWidth;
		}
	}

	//reinsert drawCtx into bast context
	ctx->insert( cmd->x, cmd->y, drawCtx );

	//remove draw context
	delete drawCtx;
}

void Renderer::render( Context* ctx, std::vector<DrawCommand*>& commands ) {
	for( DrawCommand* cmd : commands ) {
		switch( cmd->id ) {
			case DrawCommandID::GUI_DRAW_CLEAR: {
				uint32_t start_tick = xTaskGetTickCount();
				ctx->fill( 15 );
				uint32_t end_tick = xTaskGetTickCount();
				LOG_INFO("[ServiceGUI] ctx->fill( 15 ); Time: %d", end_tick - start_tick);
			}break;
			case DrawCommandID::GUI_DRAW_LINE: {

			}break;
			case DrawCommandID::GUI_DRAW_RECT: {

				uint32_t start_tick = xTaskGetTickCount();
				drawRectangle( ctx, static_cast<CommandRectangle*>(cmd) );
				uint32_t end_tick = xTaskGetTickCount();
				LOG_INFO("[ServiceGUI] drawRect Time: %d", end_tick - start_tick);

			}break;
			case DrawCommandID::GUI_DRAW_TEXT: {

				uint32_t start_tick = xTaskGetTickCount();
				drawText( ctx, static_cast<CommandText*>(cmd) );
				uint32_t end_tick = xTaskGetTickCount();
				LOG_INFO("[ServiceGUI] drawText time: %d", end_tick - start_tick);
			}break;
			case DrawCommandID::GUI_DRAW_IMAGE: {

				uint32_t start_tick = xTaskGetTickCount();
				drawImage( ctx, static_cast<CommandImage*>(cmd) );
				uint32_t end_tick = xTaskGetTickCount();
				LOG_INFO("[ServiceGUI] drawImage time: %d", end_tick - start_tick);
			} break;
			default:
				break;
		};
	}
}

} /* namespace gui */
