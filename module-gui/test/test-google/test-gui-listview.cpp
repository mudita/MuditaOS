// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"

#include "TestListView.hpp"

#include <gui/input/InputEvent.hpp>
#include <gui/widgets/ListView.hpp>
#include <test/mock/TestListViewProvider.hpp>

#include <log.hpp>

class ListViewTesting : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        testProvider = std::make_shared<gui::TestListViewProvider>();
        testListView = new TestListView(
            nullptr, testStyle::list_x, testStyle::list_y, testStyle::list_w, testStyle::list_h, nullptr);

        ASSERT_EQ(0, testListView->currentPageSize) << "List should be empty";

        testListView->setProvider(testProvider);
        testListView->rebuildList();
        testListView->focus = true;
    }

    void TearDown() override
    {
        delete testListView;
    }

    void moveNTimes(unsigned int n, gui::listview::Direction direction)
    {
        auto key = gui::KeyCode::KEY_UP;

        if (direction == gui::listview::Direction::Top)
            key = gui::KeyCode::KEY_UP;
        if (direction == gui::listview::Direction::Bottom)
            key = gui::KeyCode::KEY_DOWN;

        for (unsigned int i = 0; i < n; i++) {
            testListView->onInput(gui::InputEvent({}, gui::InputEvent::State::keyReleasedShort, key));
        }
    }

    std::shared_ptr<gui::TestListViewProvider> testProvider = nullptr;
    TestListView *testListView                              = nullptr;
};

TEST_F(ListViewTesting, Constructor_Destructor_Test)
{
    // Check that there are no memory leaks - done by fixture setup and teardown.
}

TEST_F(ListViewTesting, Fill_List_And_Item_Magin_Test)
{
    // Assign list to provider and request data - item margins set to 0, element height at 100, list height 600
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list";

    testProvider->testItemMargins = gui::Margins(0, 50, 0, 0);
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(4, testListView->currentPageSize) << "Change span size to 50 so there will be space for 4 elements";
}

TEST_F(ListViewTesting, Not_Equal_Items_Test)
{
    // Assign list to provider and request data - item margins set to 0, elements height set to 100,200,300,400... ,
    // list height 600
    testProvider->notEqualItems = true;
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(3, testListView->currentPageSize) << "3 elements should fit into list (100+200+300)";

    moveNTimes(testListView->currentPageSize, gui::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate bottom by page size - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(1, testListView->currentPageSize) << "1 elements should fit into list (400)";

    moveNTimes(1, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate top by one - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(3, testListView->currentPageSize) << "3 elements should fit into list (100+200+300)";
}

TEST_F(ListViewTesting, List_Clear_Test)
{
    // Assign list to provider and request data - item margins set to 0, element height at 100, list height 600
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 0";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";

    // Change pages
    moveNTimes(6, gui::listview::Direction::Bottom);
    moveNTimes(1, gui::listview::Direction::Top);

    // Clear list and request same data as before -> page should refresh with same data.
    testListView->reset();
    testProvider->refreshList();

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 0";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";
}

TEST_F(ListViewTesting, Scroll_Test)
{
    testListView->provider->requestRecords(0, 10);
    ASSERT_TRUE(testListView->scroll->shouldShowScroll(testListView->currentPageSize, testListView->elementsCount))
        << "10 provider elements, 100 h each, list 600 - scroll yes.";
    ASSERT_TRUE(testListView->scroll->visible) << "10 provider elements, 100 h each, list 600 - scroll yes";

    testProvider->testItemCount = 5;
    testListView->rebuildList();
    testListView->provider->requestRecords(0, 10);
    ASSERT_FALSE(testListView->scroll->shouldShowScroll(testListView->currentPageSize, testListView->elementsCount))
        << "set provider elements to 5, 100 h each, list 600 -  scroll no.";
    ASSERT_FALSE(testListView->scroll->visible) << "set provider elements to 5, 100 h each, list 600 -  scroll no";
}

TEST_F(ListViewTesting, Navigate_Test)
{
    // 10 provider elements, 100 h each, list 600.
    testListView->provider->requestRecords(0, 10);
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 0";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";

    moveNTimes(1, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached)
        << "Navigate top by 1 - page should not change as it is TopDown list - but list border reached";
    testListView->listBorderReached = false;
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 0";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";

    moveNTimes(6, gui::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate bottom by 6 - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(4, testListView->currentPageSize) << "4 elements should be displayed (10 - 6)";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 6";
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 9";
    ASSERT_EQ(gui::listview::Direction::Bottom, testListView->direction) << "List Direction should be Bottom";

    moveNTimes(1, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate top by one - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should be displayed";
    ASSERT_EQ(5, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 5";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";
    ASSERT_EQ(gui::listview::Direction::Top, testListView->direction) << "List Direction should be Top";

    moveNTimes(1, gui::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate bot by one - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(4, testListView->currentPageSize) << "4 elements should be displayed (10 - 6)";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 5";
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";
    ASSERT_EQ(gui::listview::Direction::Bottom, testListView->direction) << "List Direction should be Bottom";

    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 6 has focus";
    moveNTimes(2, gui::listview::Direction::Bottom);
    ASSERT_FALSE(testListView->listBorderReached) << "Navigate bot by 2 - page should not change";
    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 8 has focus";
    ASSERT_EQ(4, testListView->currentPageSize) << "4 elements should be displayed (10 - 6)";
    ASSERT_NE(gui::listview::Direction::Top, testListView->direction) << "list direction not changed should be Top";

    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 8 has focus";

    moveNTimes(2, gui::listview::Direction::Top);
    ASSERT_FALSE(testListView->listBorderReached) << "Navigate top by 2 - page should not change";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 6 has focus";
    ASSERT_EQ(4, testListView->currentPageSize) << "4 elements should be displayed (10 - 6)";
    ASSERT_NE(gui::listview::Direction::Top, testListView->direction) << "list direction not changed should be Top";
}

TEST_F(ListViewTesting, Continuous_Type_Test)
{
    // set list type to Continuous
    testListView->setBoundaries(gui::Boundaries::Continuous);

    // 10 provider elements, 100 h each, list 600 -> 6 elements on page.
    testListView->provider->requestRecords(0, 10);

    moveNTimes(1, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate top by one - page should change to last page";
    testListView->listBorderReached = false;
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should be displayed";

    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 9";
    ASSERT_EQ(3, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 3 (9 - 6)";
    ASSERT_EQ(gui::listview::Direction::Top, testListView->direction) << "List Direction should be Top";

    moveNTimes(testListView->currentPageSize, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate top by page size - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should be displayed";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 0";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 6";
    ASSERT_EQ(gui::listview::Direction::Bottom, testListView->direction) << "List Direction should be Bottom";

    moveNTimes(1, gui::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate bot by one - page should change";
    testListView->listBorderReached = false;
    ASSERT_EQ(4, testListView->currentPageSize) << "6 elements should be displayed";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 5";
    ASSERT_EQ(9, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";
    ASSERT_EQ(gui::listview::Direction::Bottom, testListView->direction) << "List Direction should be Bottom";

    moveNTimes(4, gui::listview::Direction::Bottom);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate bot by 4 - page should change to first page";
    testListView->listBorderReached = false;
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should be displayed";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->children.front())->ID)
        << "First element ID should be 5";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->children.back())->ID)
        << "Last element ID should be 0";
    ASSERT_EQ(gui::listview::Direction::Bottom, testListView->direction) << "List Direction should be Bottom";
}

TEST_F(ListViewTesting, Data_Deletion_Test)
{
    // Internal data -> do not delete
    testProvider->dataSource = gui::TestListViewDataSource::Internal;
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";

    // Save pointers to first and last elements
    auto pointerToFirst = dynamic_cast<gui::TestListItem *>(testListView->body->children.front());
    auto pointerToLast  = dynamic_cast<gui::TestListItem *>(testListView->body->children.back());

    // Clear list
    testListView->reset();
    ASSERT_EQ(0, testListView->body->children.size()) << "List should be empty";
    testProvider->refreshList();

    // Check if pointers match
    ASSERT_EQ(pointerToFirst, dynamic_cast<gui::TestListItem *>(testListView->body->children.front()))
        << "Pointer to first element should match";
    ASSERT_EQ(pointerToLast, dynamic_cast<gui::TestListItem *>(testListView->body->children.back()))
        << "Pointer to last element should match";

    // External data -> delete
    testProvider->dataSource = gui::TestListViewDataSource::External;
    testListView->provider->requestRecords(0, 10);

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";

    // Save pointers to first and last elements
    pointerToFirst = dynamic_cast<gui::TestListItem *>(testListView->body->children.front());
    pointerToLast  = dynamic_cast<gui::TestListItem *>(testListView->body->children.back());

    // Clear list
    testListView->reset();
    ASSERT_EQ(0, testListView->body->children.size()) << "List should be empty";
    testProvider->refreshList();

    // Check if pointers don't match
    ASSERT_NE(pointerToFirst, dynamic_cast<gui::TestListItem *>(testListView->body->children.front()))
        << "Pointer to first element should match";
    ASSERT_NE(pointerToLast, dynamic_cast<gui::TestListItem *>(testListView->body->children.back()))
        << "Pointer to last element should match";
}

TEST_F(ListViewTesting, Rebuild_Type_Test)
{
    // Internal data -> do not delete
    testProvider->dataSource = gui::TestListViewDataSource::Internal;

    // Do full list rebuild
    testListView->rebuildList(gui::listview::RebuildType::Full);

    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";
    ASSERT_EQ(0, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 0 has focus";

    moveNTimes(3, gui::listview::Direction::Bottom);
    ASSERT_FALSE(testListView->listBorderReached) << "Navigate bot by 3 - page should not change";
    ASSERT_EQ(3, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 3 has focus";

    auto pointerToFocusedElement = dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem());

    // Do in place rebuild
    testListView->rebuildList(gui::listview::RebuildType::InPlace);

    // Check if focused item did not change
    ASSERT_EQ(3, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 3 has focus";
    ASSERT_EQ(pointerToFocusedElement, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem()))
        << "Focused item should be same";

    // Do on offset rebuild on 8 element
    testListView->rebuildList(gui::listview::RebuildType::OnOffset, 8);

    // Page should change and 2 elements should fit (10 - 8)
    ASSERT_EQ(2, testListView->currentPageSize) << "2 elements should fit into list.";
    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 8 has focus";

    moveNTimes(2, gui::listview::Direction::Top);
    ASSERT_TRUE(testListView->listBorderReached) << "Navigate Top by 3 - page should change";
    ASSERT_EQ(6, testListView->currentPageSize) << "6 elements should fit into list.";
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 5 has focus";

    pointerToFocusedElement = dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem());

    // Do in place rebuild
    testListView->rebuildList(gui::listview::RebuildType::InPlace);

    // Check if focused item did not change
    ASSERT_EQ(6, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 6 has focus";
    ASSERT_EQ(pointerToFocusedElement, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem()))
        << "Focused item should be same";

    // Do full list rebuild
    testListView->rebuildList(gui::listview::RebuildType::Full);

    // Do on page element rebuild on index in Page scope
    testListView->rebuildList(gui::listview::RebuildType::OnPageElement, 3);

    // Check if focused item did not change
    ASSERT_EQ(3, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 3 has focus";

    // Do on page element rebuild in Page scope
    testListView->rebuildList(gui::listview::RebuildType::OnPageElement, 1);

    // Check if focused item did not change
    ASSERT_EQ(1, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 1 has focus";

    // Do on page element rebuild on index outside Page scope. Page should change and proper element should be focused.
    testListView->rebuildList(gui::listview::RebuildType::OnPageElement, 8);

    // Check if focused item did not change
    ASSERT_EQ(8, dynamic_cast<gui::TestListItem *>(testListView->body->getFocusItem())->ID)
        << "Check if item 8 has focus";
}
