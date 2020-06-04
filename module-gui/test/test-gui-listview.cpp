#include "catch.hpp"
#include "mock/TestListViewProvider.hpp"

TEST_CASE("List creation and deletion", "[ListView]")
{

    auto testProvider = new gui::TestListViewProvider();

    auto testListView =
        new gui::ListView(nullptr, testStyle::list_x, testStyle::list_y, testStyle::list_w, testStyle::list_h);

    testListView->setProvider(testProvider);

    REQUIRE(true);

    delete testListView;
    delete testProvider;
}
