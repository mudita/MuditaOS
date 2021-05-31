// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "visitor/GuiVisitor.hpp"
#include "Text.hpp"
#include "Item.hpp"
#include "Rect.hpp"
#include "Label.hpp"
#include "Window.hpp"
#include "BottomBar.hpp"
#include "StatusBar.hpp"
#include "ListItem.hpp"

using ::testing::Matcher;
using ::testing::Ref;

class VisitorMock : public gui::GuiVisitor
{
  public:
    MOCK_METHOD1(visit, void(gui::Item &item));
    MOCK_METHOD1(visit, void(gui::Rect &item));
    MOCK_METHOD1(visit, void(gui::Text &item));
    MOCK_METHOD1(visit, void(gui::Window &item));
    MOCK_METHOD1(visit, void(gui::Label &item));
    MOCK_METHOD1(visit, void(gui::BottomBar &item));
    MOCK_METHOD1(visit, void(gui::status_bar::StatusBar &item));
    MOCK_METHOD1(visit, void(gui::ListItem &item));
};

class CustomRect : public gui::Rect
{};

struct VisitAcceptors : public ::testing::Test
{
    gui::Item item;
    gui::Rect rect;

    gui::Text text;
    CustomRect customRect;
};

TEST_F(VisitAcceptors, VisitCallOnItem)
{
    auto &testSubject = item;
    VisitorMock visitor;
    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(1);
    testSubject.accept(visitor);
}

TEST_F(VisitAcceptors, VisitCallOnRect)
{
    auto &testSubject = rect;
    VisitorMock visitor;
    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(0);
    EXPECT_CALL(visitor, visit(Matcher<gui::Rect &>(Ref(testSubject)))).Times(1);
    testSubject.accept(visitor);
}

TEST_F(VisitAcceptors, VisitCallOnText)
{
    auto &testSubject = text;
    VisitorMock visitor;
    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(0);
    EXPECT_CALL(visitor, visit(Matcher<gui::Rect &>(Ref(testSubject)))).Times(0);
    EXPECT_CALL(visitor, visit(Matcher<gui::Text &>(Ref(testSubject)))).Times(1);
    testSubject.accept(visitor);
}

TEST_F(VisitAcceptors, VisitCallOnCustomRect)
{
    auto &testSubject = customRect;
    VisitorMock visitor;
    EXPECT_CALL(visitor, visit(Matcher<gui::Item &>(Ref(testSubject)))).Times(0);
    EXPECT_CALL(visitor, visit(Matcher<gui::Rect &>(Ref(testSubject)))).Times(1);
    testSubject.accept(visitor);
}
