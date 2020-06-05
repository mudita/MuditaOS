#include "gtest/gtest.h"
#include "../mock/TestListViewProvider.hpp"

TEST(ListView, Constructor_Destructor)
{

    auto testProvider = new gui::TestListViewProvider();

    auto testListView =
        new gui::ListView(nullptr, testStyle::list_x, testStyle::list_y, testStyle::list_w, testStyle::list_h);

    testListView->setProvider(testProvider);

    delete testListView;
    delete testProvider;
}