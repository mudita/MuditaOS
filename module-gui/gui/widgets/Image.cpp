// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "../core/DrawCommand.hpp"
#include "../core/PixMap.hpp"
#include "BoundingBox.hpp"
#include "Image.hpp"

#include "../core/ImageManager.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{

    Image::Image() : Rect(), imageMap{nullptr}
    {
        type = ItemType::IMAGE;
    }

    Image::Image(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const UTF8 imageName)
        : Rect(parent, x, y, w, h), imageMap{nullptr}
    {
        type = ItemType::IMAGE;
        set(imageName);
        setPosition(x, y);
    }

    Image::Image(const UTF8 &imageName) : imageMap{nullptr}
    {
        type = ItemType::IMAGE;
        set(imageName);
    }

    bool Image::set(int id)
    {
        imageMap = ImageManager::getInstance().getImageMap(id);
        auto w   = imageMap->getWidth();
        auto h   = imageMap->getHeight();
        setMinimumWidth(w);
        setMinimumHeight(h);
        setArea(BoundingBox(getX(), getY(), w, h));
        return true;
    }

    void Image::set(const UTF8 &name)
    {
        if (name.length()) {
            int id = ImageManager::getInstance().getImageMapID(name.c_str());
            set(id);
        }
    }

    void Image::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto img = std::make_unique<CommandImage>();
        // image
        img->x = drawArea.x;
        img->y = drawArea.y;
        img->w = drawArea.w;
        img->h = drawArea.h;
        // cmd part
        img->areaX = img->x;
        img->areaY = img->y;
        img->areaW = img->w;
        img->areaH = img->h;

        img->imageID = this->imageMap->getID();

        commands.emplace_back(std::move(img));
    }

} /* namespace gui */
