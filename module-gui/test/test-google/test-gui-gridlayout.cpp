// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"
#include <module-utils/log/log.hpp>
#include <module-gui/gui/widgets/BoxLayout.hpp>
#include <module-gui/gui/widgets/GridLayout.hpp>
#include <gui/input/InputEvent.hpp>

namespace testStyle
{
    const inline uint32_t box_x = 0;
    const inline uint32_t box_y = 0;
    const inline uint32_t box_w = 600;
    const inline uint32_t box_h = 200;

    const inline uint32_t grid_item_w = 50;
    const inline uint32_t grid_item_h = 50;
} // namespace testStyle

class TestItem : public gui::Rect
{
  public:
    unsigned int ID = 0;
    TestItem(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect(parent, x, y, w, h){};
};

class GridLayoutTesting : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        gridLayout = new gui::GridLayout(testStyle::box_x,
                                         testStyle::box_y,
                                         testStyle::box_w,
                                         testStyle::box_h,
                                         {testStyle::grid_item_w, testStyle::grid_item_h});

        ASSERT_EQ(0, gridLayout->children.size()) << "GridLayout should be empty";
    }

    void TearDown() override
    {
        delete gridLayout;
    }

    void moveNTimes(gui::BoxLayout *Box, unsigned int n, gui::KeyCode key)
    {
        for (unsigned int i = 0; i < n; i++) {
            Box->onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, key));
        }
    }

    void addNItems(gui::BoxLayout *Box,
                   unsigned int n,
                   uint32_t item_w,
                   uint32_t item_h,
                   const gui::Margins &margins = gui::Margins())
    {
        for (unsigned int i = 1; i <= n; i++) {

            auto item     = new TestItem(nullptr, 0, 0, item_w, item_h);
            item->ID      = i;
            item->visible = true;
            item->setMargins(margins);

            Box->addWidget(item);
        }
    }

    gui::GridLayout *gridLayout = nullptr;

    ///> GridLayout test constants
    const uint32_t expRowSize       = testStyle::box_h / testStyle::grid_item_h;
    const uint32_t expColSize       = testStyle::box_w / testStyle::grid_item_w;
    const uint32_t allElementsCount = expRowSize * expColSize;
};

TEST_F(GridLayoutTesting, Constructor_Destructor_Test)
{
    // Check that there are no memory leaks - done by fixture setup and teardown.
}

TEST_F(GridLayoutTesting, Fill_GridLayout_Test)
{
    for (uint32_t i = 0; i < allElementsCount; i++) {
        addNItems(gridLayout, 1, testStyle::grid_item_w, testStyle::grid_item_h);
        ASSERT_EQ(i + 1, gridLayout->children.size()) << "GridLayout should contain " << i << " elements";
        ASSERT_TRUE(gridLayout->children.back()->visible) << "Last element should be visible";
    }
    addNItems(gridLayout, 1, testStyle::grid_item_w, testStyle::grid_item_h);
    ASSERT_EQ(allElementsCount + 1, gridLayout->children.size())
        << "GridLayout should contain " << allElementsCount + 1 << " elements";
    ASSERT_FALSE(gridLayout->children.back()->visible) << "Last element shouldn't be visible";
}

TEST_F(GridLayoutTesting, Navigate_Test)
{
    ///> Test for grid layout with 48 elements
    ///> | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 |
    ///> | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 |
    ///> | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 |
    ///> | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 |
    addNItems(gridLayout, allElementsCount, testStyle::grid_item_w, testStyle::grid_item_h);
    gridLayout->setFocus(true);
    ASSERT_EQ(gridLayout->rowSize, 4) << "row size is not " << 4 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 12) << "col size is not " << 12 << " as expected";
    ASSERT_EQ(allElementsCount, gridLayout->children.size())
        << "GridLayout should contain " << allElementsCount << " elements";

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 2, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(3, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 3 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(3 + expColSize, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 3 + expColSize << " should have focus";
    moveNTimes(gridLayout, 2, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(1 + expColSize, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 + expColSize << " should have focus";
}
///> TODO: Enable this test when issue with setFocus will be resolved
TEST_F(GridLayoutTesting, DISABLED_Border_Callback_Test)
{
    ///> Test for grid layout with 46 elements
    ///> | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 |
    ///> | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 |
    ///> | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 |
    ///> | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 |
    addNItems(gridLayout, allElementsCount - 2, testStyle::grid_item_w, testStyle::grid_item_h);
    gridLayout->setFocus(true);

    ASSERT_EQ(gridLayout->rowSize, 4) << "row size is not " << 4 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 12) << "col size is not " << 12 << " as expected";
    ASSERT_EQ(allElementsCount - 2, gridLayout->children.size())
        << "GridLayout should contain " << allElementsCount - 2 << " elements";

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(37, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 37 << " should have focus";
    moveNTimes(gridLayout, 9, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(46, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 46 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(37, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 37 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(46, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 37 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(10, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 10 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(11, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 11 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(35, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 35 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(11, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 11 << " should have focus";

    ///> Test for grid layout with 1 element
    ///> | 1  |
    gridLayout->erase();
    addNItems(gridLayout, 1, testStyle::grid_item_w, testStyle::grid_item_h);
    ASSERT_EQ(gridLayout->children.size(), 1) << "elements size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->rowSize, 1) << "row size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 1) << "col size is not " << 1 << " as expected";
    gridLayout->setFocus(true);

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";

    ///> Test for grid layout with 2 elements
    ///> | 1  | 2 |
    gridLayout->erase();
    addNItems(gridLayout, 2, testStyle::grid_item_w, testStyle::grid_item_h);
    ASSERT_EQ(gridLayout->children.size(), 2) << "elements size is not " << 2 << " as expected";
    ASSERT_EQ(gridLayout->rowSize, 1) << "row size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 2) << "col size is not " << 2 << " as expected";
    gridLayout->setFocus(true);

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
}

TEST_F(GridLayoutTesting, Items_Position_Test)
{
    ///> Test for grid layout with 48 elements
    ///> | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 |
    ///> | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 |
    ///> | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 |
    ///> | 37 | 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 |
    addNItems(gridLayout, allElementsCount, testStyle::grid_item_w, testStyle::grid_item_h);
    ASSERT_EQ(allElementsCount, gridLayout->children.size()) << "GridLayout should contain 48 elements";

    for (auto item : gridLayout->children) {
        auto currColumn = (dynamic_cast<TestItem *>(item)->ID - 1) % expColSize;
        auto currRow    = (dynamic_cast<TestItem *>(item)->ID - 1) / expColSize;
        auto currPos_X  = currColumn * testStyle::grid_item_w;
        auto currPos_Y  = currRow * testStyle::grid_item_h;
        ASSERT_EQ(item->getPosition(gui::Axis::Y), currPos_Y)
            << "Y coordinate value for " << dynamic_cast<TestItem *>(item)->ID << " is invalid";
        ASSERT_EQ(item->getPosition(gui::Axis::X), currPos_X)
            << "X coordinate value for " << dynamic_cast<TestItem *>(item)->ID << " is invalid";
    }
    gridLayout->erase();
    addNItems(gridLayout, allElementsCount + 1, testStyle::grid_item_w, testStyle::grid_item_h);
    ASSERT_EQ(allElementsCount + 1, gridLayout->children.size()) << "GridLayout should contain 49 elements";

    ///> Make sure that not visible elements has not been moved
    ASSERT_EQ((*gridLayout->children.rbegin())->getPosition(gui::Axis::Y), 0);
    ASSERT_EQ((*gridLayout->children.rbegin())->getPosition(gui::Axis::X), 0);
}
