
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

#include "PixMap.hpp"
#include "VecMap.hpp"
//module-utils
#include "utf8/UTF8.hpp"

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


//43575
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

	//get copy of original context using x,y of draw coordinates and original size of the widget
	Context* drawCtx;
	bool copyContext = false;
	int16_t wgtX = 0,wgtY = 0;
	uint16_t wgtW = cmd->areaW, wgtH=cmd->areaH;
	int16_t wgtR = cmd->radius;
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
		int16_t xcBottomRight = xcTopRight;
		int16_t xcBottomLeft = xcTopLeft;

		int16_t ycTopRight = wgtY+cmd->radius+1;
		int16_t ycTopLeft = ycTopRight;
		int16_t ycBottomRight = wgtY+cmd->h - cmd->radius;
		int16_t ycBottomLeft = ycBottomRight;

		int x = cmd->radius, y = 0;

		//Calculate coordinates of the points on the circle's quarter.
		int16_t* Px = (int16_t*)malloc( 2*cmd->radius*sizeof(int16_t));
		int16_t* Py = (int16_t*)malloc( 2*cmd->radius*sizeof(int16_t));

		//if any of the vector failed to be created exit the function
		if( (Px == NULL) || (Py == NULL )) {
			if( Py ) free( Py );
			if( Px ) free( Px );
			return;
		}
		memset( Px, 0, 2*cmd->radius*sizeof(int16_t));
		memset( Py, 0, 2*cmd->radius*sizeof(int16_t));
		int32_t index = 0;
		//index where middle point of quarter of arc is located. This means that Y starts to grow faster than X
		int32_t middleIndex = 0;
		//number of points in the quater arc
		uint32_t pointCount = 0;
		Px[index] = x;
		Py[index] = y;
		++index;
		pointCount++;

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
			if (x <= y) {
				middleIndex = index;
				int idx = index;
				int xValue = Px[index];
				while( index > 0 ) {
					Px[idx] = x--;
					Py[idx] = Px[index]-2*xValue;
					++idx;
					index--;
					pointCount++;
				}
				break;
			}

			Px[index] = x;
			Py[index] = y;
			++index;
			pointCount++;
		}

		//fill the rectangle if fill flag is set using fillColor. Painting is divieded into 3 parts
		//First part takes into consideration if top corners exists, are they rounded or are they sharp.
		//Second draws simple lines between between left and right edges.
		//Third part takes into consideration if bottom corners exists, are they rounded or are they sharp.

		if( cmd->filled ) {

			//create array with offset how X position changes across Y axis
			int16_t offsetX[2*wgtR] = {0};
			uint32_t offsetXIndex = 0;

			int32_t lastY = 0x0FFFFFFF;
			for( int32_t i=0; i<int32_t(pointCount); ++i) {
				if( lastY != Py[i] ) {
					lastY = Py[i];
					offsetX[offsetXIndex] = Px[i];
					++offsetXIndex;
				}
			}

		//Part 1
			int16_t startX[2*wgtR] = {0};
			int16_t endX[2*wgtR] = {0};
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_LEFT ) {
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT ) {
					for( uint32_t i=0; i<pointCount; ++i )
						startX[i] = 0;
				}
				else {
					for( uint32_t i=0; i<pointCount; ++i )
						startX[i] = wgtR-offsetX[i];
				}
			}
			//if there is no corner set whole vector of points to the value of the radius
			else {
				for( uint32_t i=0; i<pointCount; ++i )
					startX[i] = wgtR;
			}
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT ) {
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {
					for( uint32_t i=0; i<pointCount; ++i )
						endX[i] = wgtW;
				}
				else {
					for( uint32_t i=0; i<pointCount; ++i )
						endX[i] = wgtW - wgtR + offsetX[i];
				}
			}
			//if there is no corner set whole vector of points to the value of the radius
			else {
				for( uint32_t i=0; i<pointCount; ++i )
					endX[i] = wgtW - wgtR;
			}
			for( int32_t i=0; i< wgtR; ++i )
				drawHorizontalLine( drawCtx, wgtX + startX[i], wgtY +wgtR - i, endX[i]-startX[i], 1, cmd->fillColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		//Part 2
			int16_t xs, ys;
			xs = 0;
			ys = cmd->radius;
			drawHorizontalLine( drawCtx, wgtX + xs, wgtY + ys, wgtW, wgtH - 2*cmd->radius, cmd->fillColor, LineExpansionDirection::LINE_EXPAND_DOWN );

		//Part 3
			memset( startX, 0, 2*wgtR*sizeof(int16_t));
			memset( endX, 0, 2*wgtR*sizeof(int16_t));
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_LEFT ) {
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT ) {
					for( uint32_t i=0; i<pointCount; ++i )
						startX[i] = 0;
				}
				else {
					for( uint32_t i=0; i<pointCount; ++i )
						startX[i] = wgtR-offsetX[i];
				}
			}
			//if there is no corner set whole vector of points to the value of the radius
			else {
				for( uint32_t i=0; i<pointCount; ++i )
					startX[i] = wgtR;
			}
			if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_BOTTOM_RIGHT ) {
				if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT ) {
					for( uint32_t i=0; i<pointCount; ++i )
						endX[i] = wgtW;
				}
				else {
					for( uint32_t i=0; i<pointCount; ++i )
						endX[i] = wgtW - wgtR + offsetX[i];
				}
			}
			//if there is no corner set whole vector of points to the value of the radius
			else {
				for( uint32_t i=0; i<pointCount; ++i )
					endX[i] = wgtW - wgtR;
			}
			for( int32_t i=0; i< wgtR; ++i )
				drawHorizontalLine( drawCtx, wgtX + startX[i], wgtY + wgtH -wgtR + i, endX[i]-startX[i], 1, cmd->fillColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		}

		//Here rounded border is painted

		//upper right corner
		if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT ) {
			if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {
			}
			else {
				//draw arc from 0 index up to mid point using horizontal line
				index = 0;
				//X is growing faster
				for( index=0; index<middleIndex; ++index ) {
					drawHorizontalLine( drawCtx, xcTopRight + Px[index] - cmd->penWidth, ycTopRight - Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
				}
				//Y is growing faster
				for( index=middleIndex; index<pointCount; ++index ) {
					drawVerticalLine( drawCtx, xcTopRight + Px[index], ycTopRight - Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}
		}

		//bottom right corner
		if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT ) {
			if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {
			}
			else {
				//draw arc from 0 index up to mid point using horizontal line
				index = 0;
				//X is growing faster
				for( index=0; index<middleIndex; ++index ) {
					drawHorizontalLine( drawCtx, xcBottomRight + Px[index] - cmd->penWidth, ycBottomRight + Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
				}
				//Y is growing faster
				for( index=middleIndex; index<pointCount; ++index ) {
					drawVerticalLine( drawCtx, xcBottomRight + Px[index], ycBottomRight + Py[index] - cmd->penWidth + 1, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}
		}

		//upper right corner
		if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT ) {
			if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {
			}
			else {
				//draw arc from 0 index up to mid point using horizontal line
				index = 0;
				//X is growing faster
				for( index=0; index<middleIndex; ++index ) {
					drawHorizontalLine( drawCtx, xcTopLeft - Px[index], ycTopLeft - Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
				}
				//Y is growing faster
				for( index=middleIndex; index<pointCount; ++index ) {
					drawVerticalLine( drawCtx, xcTopLeft - Px[index] + 1, ycTopLeft - Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}
		}

		//lower left corner
		if( cmd->corners & RectangleCornerFlags::GUI_RECT_CORNER_TOP_RIGHT ) {
			if( cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ) {
			}
			else {
				//draw arc from 0 index up to mid point using horizontal line
				index = 0;
				//X is growing faster
				for( index=0; index<middleIndex; ++index ) {
					drawHorizontalLine( drawCtx, xcBottomLeft - Px[index], ycBottomLeft + Py[index], cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_DOWN );
				}
				//Y is growing faster
				for( index=middleIndex; index<pointCount; ++index ) {
					drawVerticalLine( drawCtx, xcBottomLeft - Px[index] + 1, ycBottomLeft + Py[index] - cmd->penWidth + 1, cmd->penWidth, 1, cmd->borderColor, gui::LineExpansionDirection::LINE_EXPAND_LEFT );
				}
			}
		}

		free( Px );
		free( Py );


		//render edges between corners
		int16_t xs, ys,le;
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_TOP ) {
			xs = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT));
			ys = 0;
			le = wgtW - xs - cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT));
			drawHorizontalLine( drawCtx, wgtX+xs, wgtY + ys, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		}
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM ) {
			xs = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT));
			ys = wgtH;
			le = wgtW - xs - cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT));
			drawHorizontalLine( drawCtx, wgtX+xs, wgtY + ys, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_DOWN );
		}

		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_LEFT ) {
			xs = 0;
			ys = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_LEFT ));
			le = wgtH - ys - cmd->radius*(!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_LEFT));
			drawVerticalLine( drawCtx, wgtX+xs, wgtY + ys, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_RIGHT );
		}
		if( cmd->edges & RectangleEdgeFlags::GUI_RECT_EDGE_RIGHT ) {
			xs = cmd->areaW;
			ys = cmd->radius*( !(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_TOP_RIGHT ));
			le = wgtH - ys - cmd->radius*(!(cmd->flatEdges & RectangleFlatFlags::GUI_RECT_FLAT_BOTTOM_RIGHT));
			drawVerticalLine( drawCtx, wgtX+xs, wgtY + ys, le, cmd->penWidth, cmd->borderColor, LineExpansionDirection::LINE_EXPAND_LEFT );
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

	//get copy of original context using x,y of draw coordinates and original size of the widget
	Context* drawCtx;
	bool copyContext = false;
	int16_t wgtX = 0,wgtY = 0;
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
				drawChar( drawCtx, wgtX + posX + glyph->xoffset, wgtY + posY, font, glyph, cmd->color );
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
				if(( wgtX + posX + glyph->xoffset>=drawCtx->getW() ) || (wgtX + posX + glyph->xoffset < 0)) {
					LOG_FATAL( "Drawing outside context's X boundary for glyph: %d", glyph->id);
					return;
				}
				if(( wgtY + posY >= drawCtx->getH() ) || (wgtY + posY < 0)) {
					LOG_FATAL( "Drawing outside context's Y boundary for glyph: %d", glyph->id);
					return;
				}
//#endif
				drawChar( drawCtx, wgtX + posX + glyph->xoffset + kernValue, wgtY + posY, font, glyph, cmd->color );
				posX += glyph->xadvance+kernValue;

			}
		}
		idLast = idCurrent;
	}

	//if drawing was performed in temporary context
	if( copyContext) {
		//reinsert drawCtx into bast context
		ctx->insert( cmd->x, cmd->y, drawCtx );
		//remove draw context
		delete drawCtx;
	}
}

void Renderer::drawImage( Context* ctx, CommandImage* cmd ) {

	//retrive pixmap from the pixmap manager
	ImageMap* imageMap = ImageManager::getInstance().getImageMap( cmd->imageID );

	//if image is not found return;
	if( imageMap == nullptr )
		return;

	//get copy of original context using x,y of draw coordinates and original size of the widget
	Context* drawCtx = ctx->get( cmd->x, cmd->y, cmd->areaW, cmd->areaH );
	uint8_t* ctxData = drawCtx->getData();

	if( imageMap->getType() == gui::ImageMap::Type::PIXMAP ) {
		PixMap* pixMap = reinterpret_cast<PixMap*>(imageMap);
		//draw image on temporeary context
		uint32_t offsetImage = 0;
		uint32_t offsetContext = 0;
		uint32_t imageWidth = pixMap->getWidth();
		uint32_t contextWidth = drawCtx->getW();

		uint8_t* pixData = pixMap->getData();
		for( uint32_t i=0; i<pixMap->getHeight(); i++ ) {
			memcpy( ctxData + offsetContext, pixData + offsetImage, imageWidth );
			offsetImage += imageWidth;
			offsetContext += contextWidth;
		}
	}
	else if( imageMap->getType() == gui::ImageMap::Type::VECMAP) {
		VecMap* vecMap = reinterpret_cast<VecMap*>(imageMap);
		uint32_t offsetContext = 0;
		uint32_t offsetRowContext = 0;
		uint32_t imageOffset = 0;
		uint8_t alphaColor = vecMap->getAlphaColor();
		for( uint32_t row=0; row<vecMap->getHeight(); row++) {
			uint16_t vecCount = *(vecMap->getData()+imageOffset);
			imageOffset+=sizeof(uint16_t);

			offsetRowContext = offsetContext;

			for( uint32_t vec = 0; vec<vecCount; ++vec ){

				uint16_t vecOffset = *(vecMap->getData()+imageOffset);
				imageOffset+=sizeof(uint16_t);
				uint8_t vecLength = *(vecMap->getData()+imageOffset);
				imageOffset+=sizeof(uint8_t);
				uint8_t vecColor = *(vecMap->getData()+imageOffset);
				imageOffset+=sizeof(uint8_t);

				offsetRowContext += vecOffset;
				if( vecColor != alphaColor )
					memset( ctxData + offsetRowContext, vecColor, vecLength );
				offsetRowContext += vecLength;
			}
			offsetContext += drawCtx->getW();
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
//				vPortEnterCritical();
				ctx->fill( 15 );
//				vPortExitCritical();
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
