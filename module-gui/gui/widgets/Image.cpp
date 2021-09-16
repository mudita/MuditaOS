// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Image.hpp"
#include "DrawCommand.hpp"
#include "BoundingBox.hpp"
#include "ImageManager.hpp"

#include <utf8/UTF8.hpp>
#include <log.hpp>

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

    Image::Image(Item *parent, const UTF8 &imageName, ImageTypeSpecifier specifier)
        : Rect(parent, 0, 0, 0, 0), imageMap{nullptr}
    {
        type = ItemType::IMAGE;
        set(imageName, specifier);
    }

    Image::Image(const UTF8 &imageName, ImageTypeSpecifier specifier) : imageMap{nullptr}
    {
        type = ItemType::IMAGE;
        set(imageName, specifier);
    }

    bool Image::set(int id)
    {
        auto map = ImageManager::getInstance().getImageMap(id);
        if (map == nullptr) {
            LOG_ERROR("Unable to get an image map for id: %d", id);
            return false;
        }

        imageMap = map;
        auto w   = imageMap->getWidth();
        auto h   = imageMap->getHeight();
        setMinimumWidth(w);
        setMinimumHeight(h);
        setArea(BoundingBox(getX(), getY(), w, h));
        return true;
    }

    void Image::set(const UTF8 &name, ImageTypeSpecifier specifier)
    {
        if (name.empty()) {
            return;
        }

        const auto id = ImageManager::getInstance().getImageMapID(name, specifier);
        set(id);
    }

    void Image::buildDrawListImplementation(std::list<Command> &commands)
    {
        if (imageMap == nullptr) {
            LOG_ERROR("Unable to draw the image: ImageMap does not exist.");
            return;
        }

        auto img = std::make_unique<DrawImage>();
        // image
        img->origin = {drawArea.x, drawArea.y};
        // cmd part
        img->areaX   = img->origin.x;
        img->areaY   = img->origin.y;
        img->areaW   = drawArea.w;
        img->areaH   = drawArea.h;
        img->imageID = this->imageMap->getID();

        commands.emplace_back(std::move(img));
    }

    void Image::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} /* namespace gui */
