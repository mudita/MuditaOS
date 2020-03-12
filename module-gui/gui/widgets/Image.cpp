/*
 * Image.cpp
 *
 *  Created on: 17 mar 2019
 *      Author: robert
 */

#include "../core/DrawCommand.hpp"
#include "../core/PixMap.hpp"
#include "Image.hpp"

#include "../core/ImageManager.hpp"
#include "utf8/UTF8.hpp"

namespace gui {

Image::Image() : Rect(), imageMap { nullptr } {
	type = ItemType::IMAGE;
}

Image::Image( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const UTF8 imageName ) :
	Rect( parent, x, y, w, h ),
	imageMap { nullptr }{
	type = ItemType::IMAGE;
    set(imageName);
    setPosition(x, y);
	updateDrawArea();
}


Image::Image(const UTF8& imageName )
	:imageMap { nullptr }
{
	type = ItemType::IMAGE;
    set(imageName);
    updateDrawArea();
}

bool Image::set(int id)
{

    //get pixmap for selected ID
	imageMap = ImageManager::getInstance().getImageMap( id );

	//set height and width and max and min dimensions
	uint16_t imageMapWidth = imageMap->getWidth();
	uint16_t imageMapHeight = imageMap->getHeight();

	drawArea.w = imageMapWidth;
	drawArea.h = imageMapHeight;

    widgetMaxArea.w = widgetArea.w = imageMapWidth;
    widgetMaxArea.h = widgetArea.h = imageMapHeight;

    return true;
}

void Image::set(const UTF8 &name)
{
    if (name.length())
    {
        int id = ImageManager::getInstance().getImageMapID(name.c_str());
        set(id);
    }
}

std::list<DrawCommand*> Image::buildDrawList() {

	std::list<DrawCommand*> commands;

	//check if widget is visible
	if( visible == false )
		return commands;

	//get children draw commands
	std::list<DrawCommand*> childrenCommands = Item::buildDrawList();
	if( !childrenCommands.empty() )
		commands.merge( childrenCommands );

	//set local draw commands
	CommandImage* img = new CommandImage{};

	//image
	img->x = drawArea.x;
	img->y = drawArea.y;
	img->w = drawArea.w;
	img->h = drawArea.h;

	//cmd part
	img->areaX = img->x;
	img->areaY = img->y;
	img->areaW = img->w;
	img->areaH = img->h;

	img->imageID = this->imageMap->getID();

	commands.push_back( img );

	return commands;
}

} /* namespace gui */
