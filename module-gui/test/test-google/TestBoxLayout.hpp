// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gtest/gtest.h>

#include <module-gui/gui/widgets/BoxLayout.hpp>

class TestBoxLayout : public gui::BoxLayout
{

  public:
    friend class BoxLayoutTesting;

    FRIEND_TEST(BoxLayoutTesting, Constructor_Destructor_Test);
    FRIEND_TEST(BoxLayoutTesting, Fill_Box_Test);
    FRIEND_TEST(BoxLayoutTesting, Navigate_Test);
    FRIEND_TEST(BoxLayoutTesting, Border_Callback_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Alignment_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Widget_Min_Max_Resize_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Widgets_Alignment_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Widgets_Alignment_Magrin_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Margins_Test);
    FRIEND_TEST(BoxLayoutTesting, Box_Content_Change_Test);

    TestBoxLayout(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : BoxLayout(parent, x, y, w, h){};
    ~TestBoxLayout() = default;
};
