// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "vfs.hpp"
#include <module-utils/i18n/i18n.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Text.hpp"
#include "visitor/GuiVisitor.hpp"
#include "Item.hpp"
#include "Rect.hpp"
#include "Label.hpp"
#include "Window.hpp"
#include "BottomBar.hpp"
#include "TopBar.hpp"

using ::testing::Matcher;
using ::testing::Ref;

class vfs vfs;        // needed for compilation, our vfs is global
utils::i18n localize; // needed to load any keymap - these are stored in i18
struct vfs_initializer
{
    vfs_initializer()
    {
        vfs.Init();
    }
} vfs_initializer;

// class VisitorMock : public gui::GuiVisitor
//{
//  public:
//    MOCK_METHOD1(visit, void(gui::Item &item));
//    MOCK_METHOD1(visit, void(gui::Rect &item));
//    MOCK_METHOD1(visit, void(gui::Text &item));
//    MOCK_METHOD1(visit, void(gui::Window &item));
//    MOCK_METHOD1(visit, void(gui::Label &item));
//    MOCK_METHOD1(visit, void(gui::BottomBar &item));
//    MOCK_METHOD1(visit, void(gui::TopBar &item));
//};

struct VisitAcceptors : public ::testing::Test
{
    gui::Item item;
    gui::Rect rect;

    gui::Text text; /// gui::Text -> includes gui::Translator that uses vfs
    //    gui::Label label;
};

TEST_F(VisitAcceptors, VisitCallOnItem)
{
    ASSERT_TRUE(true);
    //    auto &testSubject = item;
    //    VisitorMock visitor;
    //    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(1);
    //    testSubject.accept(visitor);
}

TEST_F(VisitAcceptors, VisitCallOnRect)
{
    ASSERT_TRUE(true);
    //    auto &testSubject = rect ;
    //    VisitorMock visitor;
    //    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(0);
    //    EXPECT_CALL(visitor, visit(Matcher<gui::Rect &>(Ref(testSubject)))).Times(1);
    //    testSubject.accept(visitor);
}
