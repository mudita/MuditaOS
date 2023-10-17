// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"

#include "TestBoxLayout.hpp"

#include <log/log.hpp>
#include <module-gui/test/mock/TestListViewProvider.hpp>
#include <gui/input/InputEvent.hpp>

namespace testStyle
{
    const inline uint32_t box_x = 0;
    const inline uint32_t box_y = 0;
    const inline uint32_t box_w = 200;
    const inline uint32_t box_h = 600;

    const inline uint32_t VBox_w = 200;
    const inline uint32_t VBox_h = 600;
    const inline uint32_t HBox_w = 200;
    const inline uint32_t HBox_h = 50;

    const inline uint32_t VBox_item_w = 200;
    const inline uint32_t VBox_item_h = 100;
    const inline uint32_t HBox_item_w = 50;
    const inline uint32_t HBox_item_h = 50;
} // namespace testStyle

class TestItem : public gui::Rect
{
  public:
    unsigned int ID = 0;
    TestItem(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect(parent, x, y, w, h){};
    ~TestItem() = default;
};

class BoxLayoutTesting : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        testBoxLayout =
            new TestBoxLayout(nullptr, testStyle::box_x, testStyle::box_y, testStyle::box_w, testStyle::box_h);

        testVBoxLayout =
            new gui::VBox(nullptr, testStyle::box_x, testStyle::box_y, testStyle::VBox_w, testStyle::VBox_h);

        testHBoxLayout =
            new gui::HBox(nullptr, testStyle::box_x, testStyle::box_y, testStyle::HBox_w, testStyle::HBox_h);

        ASSERT_EQ(0, testBoxLayout->children.size()) << "Box should be empty";
        ASSERT_EQ(0, testVBoxLayout->children.size()) << "Box should be empty";
        ASSERT_EQ(0, testHBoxLayout->children.size()) << "Box should be empty";
    }

    void TearDown() override
    {
        delete testBoxLayout;
        delete testVBoxLayout;
        delete testHBoxLayout;
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

    TestItem *getNItem(gui::BoxLayout *Box, unsigned int n)
    {
        auto item = Box->children.begin();
        std::advance(item, n);

        return dynamic_cast<TestItem *>(*item);
    }

    TestBoxLayout *testBoxLayout = nullptr;
    gui::VBox *testVBoxLayout    = nullptr;
    gui::HBox *testHBoxLayout    = nullptr;

    const unsigned int fillVBoxPage     = testStyle::VBox_h / testStyle::VBox_item_h;
    const unsigned int notFillVBoxPage  = fillVBoxPage - 2;
    const unsigned int fillHBoxPage     = testStyle::HBox_w / testStyle::HBox_item_w;
    const unsigned int notFillHVBoxPage = fillHBoxPage - 1;
    const unsigned int overflowHBoxPage = fillHBoxPage + 2;
};

TEST_F(BoxLayoutTesting, Constructor_Destructor_Test)
{
    // Check that there are no memory leaks - done by fixture setup and teardown.
}

TEST_F(BoxLayoutTesting, Fill_Box_Test)
{
    // Add data to VBox and HBox
    addNItems(testVBoxLayout, fillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(fillVBoxPage, testVBoxLayout->children.size()) << "VBox should contain 6 elements";
    ASSERT_TRUE(testVBoxLayout->children.back()->visible) << "Last element should be visible";

    testVBoxLayout->erase();
    ASSERT_EQ(0, testVBoxLayout->children.size()) << "VBox should contain 0 elements";

    addNItems(testHBoxLayout, overflowHBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h);
    ASSERT_EQ(overflowHBoxPage, testHBoxLayout->children.size()) << "HBox should contain 6 elements";
    ASSERT_FALSE(getNItem(testHBoxLayout, 5)->visible) << "5 element should not be visible - as it not fit";
    ASSERT_FALSE(testHBoxLayout->children.back()->visible) << "Last element should not be visible - as it not fit";

    testHBoxLayout->erase();
    ASSERT_EQ(0, testVBoxLayout->children.size()) << "VBox should contain 0 elements";
}

TEST_F(BoxLayoutTesting, Navigate_Test)
{
    // Add data to VBox
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    testVBoxLayout->setFocus(true);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(testVBoxLayout->getFocusItem())->ID) << "first element should have focus";

    moveNTimes(testVBoxLayout, 2, gui::KeyCode::KEY_DOWN);
    ASSERT_EQ(3, dynamic_cast<TestItem *>(testVBoxLayout->getFocusItem())->ID)
        << "move down by 2 - third element should have focus";

    moveNTimes(testVBoxLayout, 1, gui::KeyCode::KEY_UP);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(testVBoxLayout->getFocusItem())->ID)
        << "move up by 1 - second element should have focus";

    // Fill HBox
    addNItems(testHBoxLayout, fillHBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h);
    ASSERT_EQ(fillHBoxPage, testHBoxLayout->children.size()) << "Box should contain 4 elements";

    testHBoxLayout->setFocus(true);
    ASSERT_EQ(1, dynamic_cast<TestItem *>(testHBoxLayout->getFocusItem())->ID) << "first element should have focus";

    moveNTimes(testHBoxLayout, 2, gui::KeyCode::KEY_RIGHT);
    ASSERT_EQ(3, dynamic_cast<TestItem *>(testHBoxLayout->getFocusItem())->ID)
        << "move right by 2 - third element should have focus";

    moveNTimes(testHBoxLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_EQ(2, dynamic_cast<TestItem *>(testHBoxLayout->getFocusItem())->ID)
        << "move left by 1 - second element should have focus";
}

TEST_F(BoxLayoutTesting, Border_Callback_Test)
{
    // Add data to VBox
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    testVBoxLayout->setFocus(true);
    auto borderCallback = false;

    testVBoxLayout->borderCallback = [&borderCallback](const gui::InputEvent &inputEvent) -> bool {
        borderCallback = true;
        return true;
    };

    moveNTimes(testVBoxLayout, 2, gui::KeyCode::KEY_DOWN);
    ASSERT_FALSE(borderCallback) << "move down by 2 - border callback should not be called";

    moveNTimes(testVBoxLayout, 2, gui::KeyCode::KEY_DOWN);
    ASSERT_TRUE(borderCallback) << "move second down time by 2 - border callback should be called";

    // Fill HBox
    addNItems(testHBoxLayout, fillHBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h);
    ASSERT_EQ(fillHBoxPage, testHBoxLayout->children.size()) << "Box should contain 4 elements";

    testHBoxLayout->setFocus(true);
    borderCallback = false;

    testHBoxLayout->borderCallback = [&borderCallback](const gui::InputEvent &inputEvent) -> bool {
        borderCallback = true;
        return true;
    };

    moveNTimes(testHBoxLayout, 1, gui::KeyCode::KEY_LEFT);
    ASSERT_TRUE(borderCallback) << "move left by 1 - border callback should be called";

    borderCallback = false;
    moveNTimes(testHBoxLayout, 2, gui::KeyCode::KEY_RIGHT);
    ASSERT_FALSE(borderCallback) << "move right by 2 - border callback should not be called";
}

TEST_F(BoxLayoutTesting, Box_Alignment_Test)
{
    // set no Reverse Order and no Alignment
    testVBoxLayout->setReverseOrder(false);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::None));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(0, testVBoxLayout->children.front()->getPosition(gui::Axis::Y)) << "first element should have Y pos 0";
    ASSERT_EQ(300, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 300";
    testVBoxLayout->erase();

    // set Reverse Order and no Alignment
    testVBoxLayout->setReverseOrder(true);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::None));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(500, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 500 - first from bottom";
    ASSERT_EQ(200, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 200";
    testVBoxLayout->erase();

    // set no Reverse Order and set Alignment to Top
    testVBoxLayout->setReverseOrder(false);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Top));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(0, testVBoxLayout->children.front()->getPosition(gui::Axis::Y)) << "first element should have Y pos 0";
    ASSERT_EQ(300, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 300";
    testVBoxLayout->erase();

    // set no Reverse Order and set Alignment to Center
    testVBoxLayout->setReverseOrder(false);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(100, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 100";
    ASSERT_EQ(400, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 400";
    testVBoxLayout->erase();

    // set no Reverse Order and set Alignment to Bottom
    testVBoxLayout->setReverseOrder(false);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Bottom));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(200, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 200";
    ASSERT_EQ(500, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 500";
    testVBoxLayout->erase();

    // set Reverse Order and set Alignment to Top
    testVBoxLayout->setReverseOrder(true);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Top));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(300, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 300";
    ASSERT_EQ(0, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 0";
    testVBoxLayout->erase();

    // set Reverse Order and set Alignment to Center
    testVBoxLayout->setReverseOrder(true);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(400, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 400";
    ASSERT_EQ(100, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 100";
    testVBoxLayout->erase();

    // set Reverse Order and set Alignment to Bottom
    testVBoxLayout->setReverseOrder(true);
    testVBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Vertical::Bottom));
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(500, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 500 - first from bottom";
    ASSERT_EQ(200, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 200";
    testVBoxLayout->erase();
}

TEST_F(BoxLayoutTesting, Box_Widget_Min_Max_Resize_Test)
{
    // Add element to HBox with 0 size
    addNItems(testHBoxLayout, 1, 0, 0);

    // Set element minimal size
    getNItem(testHBoxLayout, 0)->setMinimumSize(testStyle::HBox_item_w / 2, testStyle::HBox_item_h / 3);

    // Resize Box and check if item has actual size equal to Min size
    testHBoxLayout->resizeItems();
    ASSERT_EQ(testStyle::HBox_item_w / 2, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::X));
    ASSERT_EQ(testStyle::HBox_item_h / 3, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::Y));

    // Set element maximal size to HBoxSize
    getNItem(testHBoxLayout, 0)->setMaximumSize(testStyle::HBox_w, testStyle::HBox_h);

    // Resize Box and check if item has actual size equal to Max size
    testHBoxLayout->resizeItems();
    ASSERT_EQ(testStyle::HBox_w, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::X));
    ASSERT_EQ(testStyle::HBox_h, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::Y));

    // Set element maximal size to double HBoxSize
    getNItem(testHBoxLayout, 0)->setMaximumSize(testStyle::HBox_w * 2, testStyle::HBox_h * 2);

    // Resize Box and check if item has actual size equal to Box Size
    testHBoxLayout->resizeItems();
    ASSERT_EQ(testStyle::HBox_w, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::X));
    ASSERT_EQ(testStyle::HBox_h, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::Y));

    testHBoxLayout->erase();

    // Add element to HBox with 0 size
    addNItems(testHBoxLayout, 1, 0, 0);

    // set Box to Alignment to Right and Vertical Center
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));

    // Set element maximal size to half HBoxSize
    getNItem(testHBoxLayout, 0)->setMaximumSize(testStyle::HBox_w / 2, testStyle::HBox_h / 2);

    // Resize Box and check size
    testHBoxLayout->resizeItems();
    ASSERT_EQ(testStyle::HBox_w / 2, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::X));
    ASSERT_EQ(testStyle::HBox_h / 2, getNItem(testHBoxLayout, 0)->getSize(gui::Axis::Y));

    ASSERT_EQ(testStyle::HBox_w - testStyle::HBox_w / 2, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X));
    ASSERT_EQ((testStyle::HBox_h - testStyle::HBox_h / 2) / 2, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y));
}

TEST_F(BoxLayoutTesting, Box_Widgets_Alignment_Test)
{
    // set Box to None
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::None, gui::Alignment::Vertical::None));

    // Add 3 elements to HBox with half parent Horizontal Size
    addNItems(testHBoxLayout, notFillHVBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h / 2);
    ASSERT_EQ(notFillHVBoxPage, testHBoxLayout->children.size()) << "Box should contain 3 elements";

    // Set first item Alignment to Vertical Top
    getNItem(testHBoxLayout, 0)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Top));
    // Set second item Alignment to Vertical Center
    getNItem(testHBoxLayout, 1)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
    // Set first item Alignment to Vertical Bottom
    getNItem(testHBoxLayout, 2)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Bottom));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 0 - first from left";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y)) << "first element should have Y pos 0 - top";

    ASSERT_EQ(testStyle::HBox_item_w, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos testStyle::HBox_item_w - second from left";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/4 - Center";

    ASSERT_EQ(2 * testStyle::HBox_item_w, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 2 * testStyle::HBox_item_w - third from left";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/2 - Bottom";

    // Change Box Horizontal (in Axis) Alignment to Center
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::None));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w) / 2,
              getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 25";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y)) << "first element should have Y pos 0 - top";

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w) / 2 + testStyle::HBox_item_w,
              getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos 75";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/4 - Center";

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w) / 2 + testStyle::HBox_item_w * 2,
              getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 125";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/2 - Bottom";

    // Change Box Horizontal (in Axis) Alignment to Right
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::None));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w),
              getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 50";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y)) << "first element should have Y pos 0 - top";

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w) + testStyle::HBox_item_w,
              getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos 100";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/4 - Center";

    ASSERT_EQ((testStyle::HBox_w - notFillHVBoxPage * testStyle::HBox_item_w) + testStyle::HBox_item_w * 2,
              getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 150";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/2 - Bottom";

    // Change Box Vertical (in Axis) Alignment to Top -> so it should override children Alignment.
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 0 - first from left";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y)) << "first element should have Y pos 0 - top";

    ASSERT_EQ(testStyle::HBox_item_w, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos testStyle::HBox_item_w - second from left";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y)) << "second element should have Y pos 0 - top";

    ASSERT_EQ(2 * testStyle::HBox_item_w, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 2 * testStyle::HBox_item_w - third from left";
    ASSERT_EQ(0, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y)) << "third element should have Y pos 0 - top";

    // Change Box Vertical (in Axis) Alignment to Center -> so it should override children Alignment.
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 0 - first from left";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y))
        << "first element should have Y pos testStyle::HBox_item_h/4 - Center";

    ASSERT_EQ(testStyle::HBox_item_w, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos testStyle::HBox_item_w - second from left";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/4 - Center";

    ASSERT_EQ(2 * testStyle::HBox_item_w, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 2 * testStyle::HBox_item_w - third from left";
    ASSERT_EQ(testStyle::HBox_item_h / 4, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/4 - Center";

    // Change Box Vertical (in Axis) Alignment to Bottom -> so it should override children Alignment.
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 0 - first from left";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y))
        << "first element should have Y pos testStyle::HBox_item_h/2 - Bottom";

    ASSERT_EQ(testStyle::HBox_item_w, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos testStyle::HBox_item_w - second from left";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/2 - Bottom";

    ASSERT_EQ(2 * testStyle::HBox_item_w, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 2 * testStyle::HBox_item_w - third from left";
    ASSERT_EQ(testStyle::HBox_item_h / 2, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/2 - Bottom";
}

TEST_F(BoxLayoutTesting, Box_Widgets_Alignment_Magrin_Test)
{
    // set Box to None
    testHBoxLayout->setAlignment(gui::Alignment(gui::Alignment::Horizontal::None, gui::Alignment::Vertical::None));

    // Add 3 elements to HBox with half parent Horizontal Size
    addNItems(testHBoxLayout, notFillHVBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h / 2);
    ASSERT_EQ(notFillHVBoxPage, testHBoxLayout->children.size()) << "Box should contain 3 elements";

    // Set first item Alignment to Vertical Top
    getNItem(testHBoxLayout, 0)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Top));
    // Set second item Alignment to Vertical Center
    getNItem(testHBoxLayout, 1)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
    // Set first item Alignment to Vertical Bottom
    getNItem(testHBoxLayout, 2)->setAlignment(gui::Alignment(gui::Alignment::Vertical::Bottom));

    auto testSmallMargin = 5;
    auto testBigMargin   = 15;

    // Set items orthogonal margins
    getNItem(testHBoxLayout, 0)->setMargins(gui::Margins(0, testBigMargin, 0, 0));
    getNItem(testHBoxLayout, 1)->setMargins(gui::Margins(0, testSmallMargin, 0, testBigMargin));
    getNItem(testHBoxLayout, 2)->setMargins(gui::Margins(0, testSmallMargin, 0, testBigMargin));

    // Force position recalculation
    testHBoxLayout->resizeItems();

    ASSERT_EQ(0, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::X))
        << "first element should have X pos 0 - first from left";
    ASSERT_EQ(testBigMargin, getNItem(testHBoxLayout, 0)->getPosition(gui::Axis::Y))
        << "first element should have Y pos 0 + top margin";

    ASSERT_EQ(testStyle::HBox_item_w, getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::X))
        << "second element should have X pos testStyle::HBox_item_w - second from left";
    ASSERT_EQ(testStyle::HBox_item_h / 4 - ((testSmallMargin + testBigMargin) / 2),
              getNItem(testHBoxLayout, 1)->getPosition(gui::Axis::Y))
        << "second element should have Y pos testStyle::HBox_item_h/4 - margins/2";

    ASSERT_EQ(2 * testStyle::HBox_item_w, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::X))
        << "third element should have X pos 2 * testStyle::HBox_item_w - third from left";
    ASSERT_EQ(testStyle::HBox_item_h / 2 - testBigMargin, getNItem(testHBoxLayout, 2)->getPosition(gui::Axis::Y))
        << "third element should have Y pos testStyle::HBox_item_h/2 - bottom margin";
}

TEST_F(BoxLayoutTesting, Box_Margins_Test)
{
    auto testTopMargin    = 30;
    auto testBottomMargin = 50;
    auto testRightMargin  = 2;
    auto testLeftMargin   = 5;

    // Add data to VBox with custom margins top 30, bot 50.
    addNItems(testVBoxLayout,
              fillVBoxPage,
              testStyle::VBox_item_w,
              testStyle::VBox_item_h,
              gui::Margins(0, testTopMargin, 0, testBottomMargin));

    ASSERT_EQ(testTopMargin, getNItem(testVBoxLayout, 0)->widgetArea.y) << "First element y pos should be 30";
    ASSERT_EQ(testTopMargin + testStyle::VBox_item_h + testBottomMargin + testTopMargin,
              getNItem(testVBoxLayout, 1)->widgetArea.y)
        << "Second element y pos should be 210";

    // Add data to HBox with custom margins right 20, left 50 and with reverse order.
    testHBoxLayout->setReverseOrder(true);
    addNItems(testHBoxLayout,
              fillHBoxPage,
              testStyle::HBox_item_w,
              testStyle::HBox_item_h,
              gui::Margins(testLeftMargin, 0, testRightMargin, 0));

    ASSERT_EQ(testStyle::HBox_w - testRightMargin - testStyle::HBox_item_w, getNItem(testHBoxLayout, 0)->widgetArea.x)
        << "First element y pos should be 148";
    ASSERT_EQ(testStyle::HBox_w - testRightMargin - testStyle::HBox_item_w - testLeftMargin - testRightMargin -
                  testStyle::HBox_item_w,
              getNItem(testHBoxLayout, 1)->widgetArea.x)
        << "Second element y pos should be 91";
}

TEST_F(BoxLayoutTesting, Box_Content_Change_Test)
{
    auto thirdBox  = new gui::HBox(nullptr, testStyle::box_x, testStyle::box_y, testStyle::HBox_w, testStyle::HBox_h);
    auto secondBox = new gui::HBox(thirdBox, testStyle::box_x, testStyle::box_y, testStyle::HBox_w, testStyle::HBox_h);
    auto firstBox  = new gui::HBox(secondBox, testStyle::box_x, testStyle::box_y, testStyle::HBox_w, testStyle::HBox_h);

    // Fill first box with data
    addNItems(firstBox, fillHBoxPage, testStyle::HBox_item_w, testStyle::HBox_item_h);

    // Check Boxes content
    ASSERT_EQ(firstBox->children.size(), 4) << "First box element size should be 4";
    ASSERT_TRUE(getNItem(firstBox, 0)->visible) << "First box element 0 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 1)->visible) << "First box element 1 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 2)->visible) << "First box element 2 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 3)->visible) << "First box element 3 should be visible";

    // Change first box, first element min/max size to parent size.
    getNItem(firstBox, 0)->setMinimumSize(testStyle::HBox_w, testStyle::HBox_h);
    getNItem(firstBox, 0)->setMaximumSize(testStyle::HBox_w, testStyle::HBox_h);

    // Box contents should not change as no automatic recalculate procedure has been called
    ASSERT_EQ(firstBox->children.size(), 4) << "First box element size should be 4";
    ASSERT_TRUE(getNItem(firstBox, 0)->visible) << "First box element 0 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 1)->visible) << "First box element 1 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 2)->visible) << "First box element 2 should be visible";
    ASSERT_TRUE(getNItem(firstBox, 3)->visible) << "First box element 3 should be visible";

    // Call content change method on firstBox
    firstBox->informContentChanged();

    // Box contents should have changed and only one element should be visible
    ASSERT_EQ(firstBox->children.size(), 4) << "First box element size should be 4";
    ASSERT_TRUE(getNItem(firstBox, 0)->visible) << "First box element 0 should be visible";
    ASSERT_FALSE(getNItem(firstBox, 1)->visible) << "First box element 1 should not be visible";
    ASSERT_FALSE(getNItem(firstBox, 2)->visible) << "First box element 2 should not be visible";
    ASSERT_FALSE(getNItem(firstBox, 3)->visible) << "First box element 3 should not be visible";

    delete thirdBox;
}
