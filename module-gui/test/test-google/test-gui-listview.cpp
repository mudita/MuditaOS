#include "gtest/gtest.h"
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-utils/log/log.hpp>
#include "../mock/TestListViewProvider.hpp"

class TestListView : public gui::ListView
{

  public:
    friend class ListViewTesting;
    FRIEND_TEST(ListViewTesting, Constructor_Destructor_Test);
    FRIEND_TEST(ListViewTesting, Fill_List_And_Span_Test);
    FRIEND_TEST(ListViewTesting, Scroll_Test);
    FRIEND_TEST(ListViewTesting, Navigate_Test);
    FRIEND_TEST(ListViewTesting, Continuous_Type_Test);

    bool pageChanged = false;

    bool listPageEndReached() override
    {
        pageChanged = true;
        return ListView::listPageEndReached();
    }

    TestListView(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : ListView(parent, x, y, w, h){};
    ~TestListView() = default;
};

class ListViewTesting : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        testProvider = new gui::TestListViewProvider();
        testListView =
            new TestListView(nullptr, testStyle::list_x, testStyle::list_y, testStyle::list_w, testStyle::list_h);

        // Check if list is empty
        ASSERT_EQ(0, testListView->currentPageSize);
        testListView->setProvider(testProvider);

        // Set span size to 0 for easy test calculation.
        testListView->setItemSpanSize(0);

        LOG_DEBUG("Constructing list and provider");
    }

    void TearDown() override
    {
        delete testListView;
        delete testProvider;

        LOG_DEBUG("Deleting list and provider");
    }

    void MoveNTimes(unsigned int n, style::listview::Direction direction)
    {
        auto key = gui::KeyCode::KEY_UP;

        if (direction == style::listview::Direction::Top)
            key = gui::KeyCode::KEY_UP;
        if (direction == style::listview::Direction::Bottom)
            key = gui::KeyCode::KEY_DOWN;

        for (unsigned int i = 0; i < n; i++) {
            testListView->onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, key));
        }
    }

    gui::TestListViewProvider *testProvider = nullptr;
    TestListView *testListView              = nullptr;
};

TEST_F(ListViewTesting, Constructor_Destructor_Test)
{
    // Check that there are no memory leaks - done by fixture setup and teardown.
}

TEST_F(ListViewTesting, Fill_List_And_Span_Test)
{
    // Assign list to provider and request data - span set to 0, element height at 100, list height 600 -> 6 elements
    // should fit into list.

    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(6, testListView->currentPageSize);

    // Change span size to 50 so there will be space for 4 elements

    testListView->setItemSpanSize(50);
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(4, testListView->currentPageSize);
}

TEST_F(ListViewTesting, Scroll_Test)
{
    // 10 provider elements, 100 h each, list 600 - scroll yes.
    testListView->provider->requestRecords(0, 10);
    ASSERT_TRUE(testListView->scroll->shouldShowScroll(testListView->currentPageSize, testListView->elementsCount));
    ASSERT_TRUE(testListView->scroll->visible);

    // set provider elements to 5, 100 h each, list 600 -  scroll no.
    testProvider->testItemCount = 5;
    testListView->provider->requestRecords(0, 10);
    ASSERT_FALSE(testListView->scroll->shouldShowScroll(testListView->currentPageSize, testListView->elementsCount));
    ASSERT_FALSE(testListView->scroll->visible);
}

TEST_F(ListViewTesting, Navigate_Test)
{
    // 10 provider elements, 100 h each, list 600 -> 6 elements on page.
    testListView->provider->requestRecords(0, 10);
    ASSERT_EQ(6, testListView->currentPageSize);

    // Navigate bottom by 6 - page should change
    MoveNTimes(6, style::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 4 elements should be displayed (10 - 6)
    ASSERT_EQ(4, testListView->currentPageSize);
    // First element ID should be 6
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 9
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Bottom, testListView->direction);

    // Navigate top by one - page should change
    MoveNTimes(1, style::listview::Direction::Top);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 6 elements should be displayed
    ASSERT_EQ(6, testListView->currentPageSize);
    // First element ID should be 5
    ASSERT_EQ(5, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 0
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Top, testListView->direction);

    // Navigate bot by one - page should change
    MoveNTimes(1, style::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 4 elements should be displayed (10 - 6)
    ASSERT_EQ(4, testListView->currentPageSize);
    // First element ID should be 5
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 0
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Bottom, testListView->direction);

    // Check if item 6 has focus
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID);
    // Navigate bot by 2 - page should not change
    MoveNTimes(2, style::listview::Direction::Bottom);
    // Check if item 8 has focus
    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID);
    ASSERT_FALSE(testListView->pageChanged);
    // 4 elements should be displayed (10 - 6)
    ASSERT_EQ(4, testListView->currentPageSize);
    // Check list direction not changed
    ASSERT_NE(style::listview::Direction::Top, testListView->direction);

    // Check if item 8 has focus
    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID);
    // Navigate top by 2 - page should not change
    MoveNTimes(2, style::listview::Direction::Top);
    // Check if item 6 has focus
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID);
    ASSERT_FALSE(testListView->pageChanged);
    // 4 elements should be displayed (10 - 6)
    ASSERT_EQ(4, testListView->currentPageSize);
    // Check list direction not changed
    ASSERT_NE(style::listview::Direction::Top, testListView->direction);
}

TEST_F(ListViewTesting, Continuous_Type_Test)
{
    // set list type to Continuous
    testListView->setListViewType(style::listview::Type::Continuous);

    // 10 provider elements, 100 h each, list 600 -> 6 elements on page.
    testListView->provider->requestRecords(0, 10);

    // Navigate top by one - page should change to last page
    MoveNTimes(1, style::listview::Direction::Top);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 4 elements should be displayed
    ASSERT_EQ(4, testListView->currentPageSize);
    // First element ID should be 9
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 3 (9 - 6)
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Top, testListView->direction);

    // Navigate top by page size - page should change
    MoveNTimes(testListView->currentPageSize, style::listview::Direction::Top);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 6 elements should be displayed
    ASSERT_EQ(6, testListView->currentPageSize);
    // First element ID should be 5
    ASSERT_EQ(5, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 0
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Top, testListView->direction);

    // Navigate bot by one - page should change
    MoveNTimes(1, style::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 6 elements should be displayed
    ASSERT_EQ(4, testListView->currentPageSize);
    // First element ID should be 5
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 0
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Bottom, testListView->direction);

    // Navigate bot by 4 - page should change to first page
    MoveNTimes(4, style::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->pageChanged);
    testListView->pageChanged = false;
    // 6 elements should be displayed
    ASSERT_EQ(6, testListView->currentPageSize);
    // First element ID should be 5
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID);
    // Last element ID should be 0
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID);
    // Check list direction
    ASSERT_EQ(style::listview::Direction::Bottom, testListView->direction);
}