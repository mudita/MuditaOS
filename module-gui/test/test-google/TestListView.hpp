// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gtest/gtest.h>

#include <gui/widgets/ListView.hpp>

class TestListView : public gui::ListView
{

  public:
    friend class ListViewTesting;
    FRIEND_TEST(ListViewTesting, Constructor_Destructor_Test);
    FRIEND_TEST(ListViewTesting, Fill_List_And_Item_Magin_Test);
    FRIEND_TEST(ListViewTesting, Not_Equal_Items_Test);
    FRIEND_TEST(ListViewTesting, List_Clear_Test);
    FRIEND_TEST(ListViewTesting, Scroll_Test);
    FRIEND_TEST(ListViewTesting, Navigate_Test);
    FRIEND_TEST(ListViewTesting, Continuous_Type_Test);
    FRIEND_TEST(ListViewTesting, Data_Deletion_Test);
    FRIEND_TEST(ListViewTesting, Rebuild_Type_Test);

    bool listBorderReached = false;

    bool requestNextPage() override
    {
        listBorderReached = true;
        return ListView::requestNextPage();
    }

    bool requestPreviousPage() override
    {
        listBorderReached = true;
        return ListView::requestPreviousPage();
    }

    TestListView(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<gui::ListItemProvider> prov)
        : ListView(parent, x, y, w, h, prov){};
    ~TestListView() = default;
};
