// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageBox.hpp"

using namespace gui;

ImageBox::ImageBox(Item *parent, const Position &x, const Position &y, const Length &w, const Length &h, Image *image)
    : HBox(parent, x, y, w, h), image(image)
{
    setEdges(RectangleEdge::None);
    setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    addWidget(image);
}

ImageBox::ImageBox(Item *parent, Image *image) : ImageBox(parent, 0, 0, 0, 0, image)
{}

void ImageBox::showImage(bool show)
{
    image->setVisible(show);
}

void ImageBox::setImage(const UTF8 &name)
{
    image->set(name);
}

void ImageBox::setMinimumSizeToFitImage()
{
    setMinimumSize(image->getWidth(), image->getHeight());
}
