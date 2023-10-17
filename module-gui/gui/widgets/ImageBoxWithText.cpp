// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageBoxWithText.hpp"

using namespace gui;

ImageBoxWithText::ImageBoxWithText(Item *parent,
                                   const Position &x,
                                   const Position &y,
                                   const Length &w,
                                   const Length &h,
                                   Image *image,
                                   const UTF8 &description)
    : ImageBox(parent, x, y, w, h, image)
{
    setText(description);
    setMinimumSize(imageBoxWithText::wh, imageBoxWithText::wh);
    image->setEdges(RectangleEdge::All);

    focusChangedCallback = [&](Item &item) {
        if (focus) {
            setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
        }
        else {
            setEdges(RectangleEdge::None);
        }
        return true;
    };
}

ImageBoxWithText::ImageBoxWithText(Item *parent, Image *image, const UTF8 &description)
    : ImageBoxWithText(parent, 0, 0, 0, 0, image, description)
{}

void ImageBoxWithText::setText(const UTF8 &description)
{
    text = new TextFixedSize(this);
    text->drawUnderline(false);
    text->setMargins(Margins(0, imageBoxWithText::text_margin_top, 0, imageBoxWithText::text_margin_bottom));
    text->setFont(imageBoxWithText::font);
    text->setText(description);
    text->setMinimumWidthToFitText(description);
    text->setMinimumHeightToFitText();
    text->setAlignment(Alignment::Horizontal::Center);
    text->activeItem = false;
}

void ImageBoxWithText::setMinimumSizeToFitImage()
{
    auto minW = std::max(image->getWidth(), text->widgetMinimumArea.w);
    auto minH = image->getHeight() + imageBoxWithText::text_margin_top + text->widgetMinimumArea.h;
    setMinimumSize(minW, minH);
}
