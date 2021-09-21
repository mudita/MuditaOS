// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"
#include <log/log.hpp>
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
    void addItem(gui::BoxLayout *Box,
                 uint32_t item_w,
                 uint32_t item_h,
                 uint32_t id,
                 bool active                 = true,
                 const gui::Margins &margins = gui::Margins())
    {
        auto item     = new TestItem(nullptr, 0, 0, item_w, item_h);
        item->ID      = id;
        item->visible = true;
        item->setMargins(margins);
        item->activeItem = active;
        Box->addWidget(item);
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

TEST_F(GridLayoutTesting, Navigate_Test_ActiveItems_1)
{
    ///> Test for grid layout with 48 elements (A - active item, N - non active item, NV - non visible item)
    ///> | 1   A | 2  NA | 3   A | 4  NA | 5   A | 6  NA | 7  A  | 8  NA | 9   A | 10 NA | 11 A  | 12 NA |
    ///> | 13 NA | 14  A | 15 NA | 16  A | 17 NA | 18  A | 19 NA | 20  A | 21 NA | 22  A | 23 NA | 24  A |
    ///> | 25  A | 26 NA | 27  A | 28 NA | 29  A | 30 NA | 31 A  | 32 NA | 33  A | 34 NA | 35 A  | 36 NA |
    ///> | 37 NA | 38  A | 39 NA | 40  A | 41 NA | 42  A | 43 NA | 44  A | 45 NA | 46  A | 47 NA | 48  A |
    ///> | 49 NV | 50 NV | 51 NV | 52 NV |
    for (uint32_t i = 1; i <= 12; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, (i % 2) ? true : false);
    }
    for (uint32_t i = 13; i <= 24; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, ((i + 1) % 2) ? true : false);
    }
    for (uint32_t i = 25; i <= 36; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, (i % 2) ? true : false);
    }
    for (uint32_t i = 37; i <= 48; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, ((i + 1) % 2) ? true : false);
    }
    ///> Add some items to exceed grid layout area
    for (uint32_t i = 49; i <= 52; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, true);
    }
    gridLayout->setFocus(true);
    ASSERT_EQ(gridLayout->rowSize, 4) << "row size is not " << 4 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 12) << "col size is not " << 12 << " as expected";
    ASSERT_EQ(52, gridLayout->children.size()) << "GridLayout should contain " << 52 << " elements";

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 2, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(5, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 5 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(5 + (2 * expColSize), dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 5 + (2 * expColSize) << " should have focus";
    moveNTimes(gridLayout, 2, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(1 + (2 * expColSize), dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 + (2 * expColSize) << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
}

TEST_F(GridLayoutTesting, Navigate_Test_ActiveItems_2_BorderCallback)
{
    ///> Test for grid layout with 48 elements
    ///> | 1   A | 2  NA | 3   A | 4  NA | 5   A | 6  NA | 7  A  | 8  NA | 9   A | 10 NA | 11 A  | 12 NA |
    ///> | 13 NA | 14  A | 15 NA | 16  A | 17 NA | 18  A | 19 NA | 20  A | 21 NA | 22  A | 23 NA | 24  A |
    ///> | 25  A | 26 NA | 27  A | 28 NA | 29  A | 30 NA | 31 A  | 32 NA | 33  A | 34 NA | 35 A  | 36 NA |
    ///> | 37 NA | 38  A | 39 NA |
    for (uint32_t i = 1; i <= 12; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, (i % 2) ? true : false);
    }
    for (uint32_t i = 13; i <= 24; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, ((i + 1) % 2) ? true : false);
    }
    for (uint32_t i = 25; i <= 36; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, (i % 2) ? true : false);
    }
    for (uint32_t i = 37; i <= 39; i++) {
        addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, i, ((i + 1) % 2) ? true : false);
    }

    gridLayout->setFocus(true);
    ASSERT_EQ(gridLayout->rowSize, 4) << "row size is not " << 4 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 12) << "col size is not " << 12 << " as expected";
    ASSERT_EQ(39, gridLayout->children.size()) << "GridLayout should contain " << 39 << " elements";

    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";
    moveNTimes(gridLayout, 2, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(9, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 9 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(33, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 33 << " should have focus";
    moveNTimes(gridLayout, 3, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(27, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 27 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(3, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 3 << " should have focus";
    moveNTimes(gridLayout, 5, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 1 << " should have focus";

    ///> Test for grid layout with 1 element
    ///> | 1NA |
    gridLayout->erase();
    addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, 1, false);
    ASSERT_EQ(gridLayout->children.size(), 1) << "elements size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->rowSize, 1) << "row size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 1) << "col size is not " << 1 << " as expected";
    gridLayout->setFocus(false);
    gridLayout->setFocus(true);
    ASSERT_EQ(nullptr, gridLayout->getFocusItem()) << "no element shall be focused";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(nullptr, gridLayout->getFocusItem()) << "no element shall be focused";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(nullptr, gridLayout->getFocusItem()) << "no element shall be focused";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(nullptr, gridLayout->getFocusItem()) << "no element shall be focused";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(nullptr, gridLayout->getFocusItem()) << "no element shall be focused";
    ///> Test for grid layout with 1 element
    ///> | 1NA | 1A |
    gridLayout->erase();
    addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, 1, false);
    addItem(gridLayout, testStyle::grid_item_w, testStyle::grid_item_h, 2, true);
    ASSERT_EQ(gridLayout->children.size(), 2) << "elements size is not " << 2 << " as expected";
    ASSERT_EQ(gridLayout->rowSize, 1) << "row size is not " << 1 << " as expected";
    ASSERT_EQ(gridLayout->colSize, 2) << "col size is not " << 2 << " as expected";
    gridLayout->setFocus(false);
    gridLayout->setFocus(true);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
    moveNTimes(gridLayout, 1, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(gridLayout->getFocusItem())->ID)
        << "element with ID " << 2 << " should have focus";
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
        << "element with ID " << 46 << " should have focus";
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
