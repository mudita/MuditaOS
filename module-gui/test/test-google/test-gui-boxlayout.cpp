#include "gtest/gtest.h"
#include <module-utils/log/log.hpp>
#include <module-gui/gui/widgets/BoxLayout.hpp>

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

class TestBoxLayout : public gui::BoxLayout
{

  public:
    friend class BoxLayoutTesting;
    FRIEND_TEST(BoxLayoutTesting, Constructor_Destructor_Test);
    FRIEND_TEST(BoxLayoutTesting, Fill_Box_Test);
    FRIEND_TEST(BoxLayoutTesting, Navigate_Test);
    FRIEND_TEST(BoxLayoutTesting, Border_Callback_Test);
    FRIEND_TEST(BoxLayoutTesting, Axis_Alignment_Test);
    FRIEND_TEST(BoxLayoutTesting, Item_Margins_Test);

    TestBoxLayout(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : BoxLayout(parent, x, y, w, h){};
    ~TestBoxLayout() = default;
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

TEST_F(BoxLayoutTesting, Axis_Alignment_Test)
{
    // set no Reverse Order
    testVBoxLayout->setReverseOrder(false);

    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(0, testVBoxLayout->children.front()->getPosition(gui::Axis::Y)) << "first element should have Y pos 0";
    ASSERT_EQ(300, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 300";
    testVBoxLayout->erase();

    // set Reverse Order
    testVBoxLayout->setReverseOrder(true);
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";

    ASSERT_EQ(500, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 500 - first from bottom";
    ASSERT_EQ(200, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 200";
    testVBoxLayout->erase();

    // set Reverse Order and call axis Alignment
    testVBoxLayout->setReverseOrder(true);
    // Add 4 elements to VBox - there should be space for 6
    addNItems(testVBoxLayout, notFillVBoxPage, testStyle::VBox_item_w, testStyle::VBox_item_h);
    ASSERT_EQ(notFillVBoxPage, testVBoxLayout->children.size()) << "Box should contain 4 elements";
    testVBoxLayout->axisAlignment();

    ASSERT_EQ(300, testVBoxLayout->children.front()->getPosition(gui::Axis::Y))
        << "first element should have Y pos 300";
    ASSERT_EQ(0, testVBoxLayout->children.back()->getPosition(gui::Axis::Y)) << "last element should have Y pos 0";
}

TEST_F(BoxLayoutTesting, Item_Margins_Test)
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
