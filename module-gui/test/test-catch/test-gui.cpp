// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/// These are random tests what could be salvaged from old tests

#include "mock/InitializedFontManager.hpp"
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file

#include <memory>
#include <functional>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <catch2/catch.hpp>

#include <log/log.hpp>
#include <utf8/UTF8.hpp>

#include <module-gui/gui/core/ImageManager.hpp>
#include <module-gui/gui/core/BoundingBox.hpp>
#include <module-gui/gui/widgets/text/Label.hpp>
#include <module-gui/gui/widgets/BoxLayout.hpp>
#include <module-gui/gui/widgets/Image.hpp>

#include <mock/TestWindow.hpp>

using namespace std;

TEST_CASE("Test BoundingBox intersect")
{
    gui::BoundingBox result;
    REQUIRE(gui::BoundingBox::intersect(gui::BoundingBox(0, 0, 15, 15), gui::BoundingBox(15, 0, 15, 15), result) ==
            false);
    REQUIRE(gui::BoundingBox::intersect(gui::BoundingBox(0, 0, 15, 15), gui::BoundingBox(0, 15, 15, 15), result) ==
            false);
    REQUIRE(gui::BoundingBox::intersect(gui::BoundingBox(0, 0, 15, 15), gui::BoundingBox(-15, 0, 15, 15), result) ==
            false);
    REQUIRE(gui::BoundingBox::intersect(gui::BoundingBox(0, 0, 15, 15), gui::BoundingBox(0, -15, 15, 15), result) ==
            false);
    REQUIRE(gui::BoundingBox::intersect(gui::BoundingBox(0, 0, 15, 15), gui::BoundingBox(14, 0, 15, 15), result) ==
            true);
}

/// note that fontmanager is `global` so loading it now will make it available in next steps
/// which is why it makes next steps work on possibly empty element (fontmanager)
/// tbh - there should allways be fallback to some memory stored font in our FontManager
TEST_CASE("Are fonts loaded")
{
    auto &fontmanager = gui::FontManager::getInstance();
    // check getInstance - getting even default font will result in nullptr
    // this is because no fonts are loaded
    REQUIRE(fontmanager.getFont() == nullptr);
    // now initialize, from where is it taken? nobody knows from this foo
    fontmanager.init("sys/current/assets");
    // check if there is at least default font
    REQUIRE(fontmanager.getFont() != nullptr);
}

TEST_CASE("Draw window with labels")
{
    mockup::fontManager(); // To load fonts
    auto win = new gui::TestWindow("MAIN");
    win->setSize(480, 600);

    // add label with time
    auto label = new gui::Label(win, 20, 80, 440, 120, "TOP LEFT");
    label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
    label->setRadius(5);
    label->setFont("gt_pressura_bold_65");

    label = new gui::Label(win, 20, 280, 440, 120, "2xCENTER#####");
    label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    label->setRadius(10);
    label->setPenWidth(5);
    label->setFont("gt_pressura_bold_65");

    label = new gui::Label(win, 20, 480, 440, 120, "BOTTOM RIGHT");
    label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Bottom));
    label->setRadius(15);
    label->setTextEllipsisType(gui::TextEllipsis::Right);
    label->setFont("gt_pressura_bold_65");

    // vector with draw commands
    // context for drawing commands
    auto commandsList = win->buildDrawList();

    delete win;
}

TEST_CASE("Draw window with box layouts")
{
    mockup::fontManager();
    auto win_unique = make_unique<gui::TestWindow>("MAIN"); // make root gui element used unique to auto clean it
    auto win        = win_unique.get();                     // get pointer to root element for ctros sake
    win->setSize(480, 600);

    gui::HBox *hBox = new gui::HBox(win, 50, 50, 380, 500);

    gui::Rect *maxW1 = new gui::Rect();
    maxW1->setFillColor(gui::Color(5, 0));
    maxW1->setFilled(true);
    maxW1->setMaximumSize(50, 300);

    gui::Label *maxW4 = new gui::Label(nullptr);
    maxW4->setText("Top Left corner");
    maxW4->setTextEllipsisType(gui::TextEllipsis::Right);
    maxW4->setMaximumSize(275, 60);

    gui::Rect *maxW2 = new gui::Rect();
    maxW2->setFillColor(gui::Color(8, 0));
    maxW2->setFilled(true);
    maxW2->setMaximumSize(35, 300);

    gui::Rect *maxW3 = new gui::Rect();
    maxW3->setFillColor(gui::Color(11, 0));
    maxW3->setFilled(true);
    maxW3->setMaximumSize(30, 300);

    hBox->addWidget(maxW1);
    hBox->addWidget(maxW4);
    hBox->addWidget(maxW2);

    gui::VBox *vBox = new gui::VBox(hBox, 10, 155, 460, 600 - 160);

    gui::Rect *maxH1 = new gui::Rect();
    maxH1->setMaximumSize(10, 80);

    gui::Rect *maxH2 = new gui::Rect();
    maxH2->setMaximumSize(15, 300);

    gui::Rect *maxH3 = new gui::Rect();
    maxH3->setMaximumSize(30, 300);

    gui::Label *maxH4 = new gui::Label(nullptr);
    maxH4->setText("Hello Mudita");
    maxH4->setRadius(20);
    maxH4->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    maxH4->setMaximumSize(75, 60);

    vBox->addWidget(maxH1);
    vBox->addWidget(maxH2);
    vBox->addWidget(maxH4);
    vBox->addWidget(maxH3);

    hBox->addWidget(maxW3);
}
