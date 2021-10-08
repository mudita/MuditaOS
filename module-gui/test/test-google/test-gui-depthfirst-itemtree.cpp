// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"

#include "visitor/DepthFirstItemTree.hpp"
#include "Item.hpp"

#include <vector>
#include <iostream>

class DepthFirstTreeTesting : public ::testing::Test
{

    gui::Item root;
    std::vector<gui::Item *> firstLevelChildren;
    std::vector<gui::Item *> secondLevelChildren;

    void addItem(gui::Item *parent, std::vector<gui::Item *> &childrenList)
    {
        auto item = new gui::Item();
        parent->addWidget(item);
        childrenList.push_back(item);
    }

  protected:
    DepthFirstTreeTesting(gui::DepthFirstItemTree::TraverseMode mode)
    {
        // adding 1st-level children to root
        addItem(&root, firstLevelChildren);
        addItem(&root, firstLevelChildren);
        addItem(&root, firstLevelChildren);

        // adding 2nd-level children
        auto child = firstLevelChildren.back();
        addItem(child, secondLevelChildren);
        addItem(child, secondLevelChildren);
        addItem(child, secondLevelChildren);
        child = firstLevelChildren.front();
        addItem(child, secondLevelChildren);
        addItem(child, secondLevelChildren);
        addItem(child, secondLevelChildren);

        tree = std::make_unique<gui::DepthFirstItemTree>(root, mode);
    }

  public:
    static constexpr auto rootLevel   = 0;
    static constexpr auto firstLevel  = 1;
    static constexpr auto secondLevel = 2;

    std::unique_ptr<gui::DepthFirstItemTree> tree;

    gui::Item *getRoot()
    {
        return &root;
    }
    const std::vector<gui::Item *> &get1stLevelChildren() const
    {
        return firstLevelChildren;
    }
    const std::vector<gui::Item *> &get2ndtLevelChildren() const
    {
        return secondLevelChildren;
    }
};

class DFTPreOrderTesting : public DepthFirstTreeTesting
{
  protected:
    DFTPreOrderTesting() : DepthFirstTreeTesting(gui::DepthFirstItemTree::TraverseMode::PreOrder)
    {}
};

class DFTPostOrderTesting : public DepthFirstTreeTesting
{

  protected:
    DFTPostOrderTesting() : DepthFirstTreeTesting(gui::DepthFirstItemTree::TraverseMode::PostOrder)
    {}
};

TEST_F(DFTPreOrderTesting, RootAtFrontTest)
{
    ASSERT_TRUE(tree->hasNext());
    auto node = tree->getNext();
    ASSERT_EQ(node.getLevel(), rootLevel);
    ASSERT_EQ(&node.getItem(), getRoot());
}

TEST_F(DFTPreOrderTesting, SecondLevelChildAtEndTest)
{
    ASSERT_TRUE(tree->hasNext());
    while (tree->hasNext()) {
        auto node = tree->getNext();
        if (!tree->hasNext()) {
            ASSERT_EQ(node.getLevel(), secondLevel);
        }
    }
}

TEST_F(DFTPreOrderTesting, ExpectedNodesCountTest)
{
    const int expectedCount = 1 + get1stLevelChildren().size() + get2ndtLevelChildren().size();
    int currentCount        = 0;
    while (tree->hasNext()) {
        auto node = tree->getNext();
        EXPECT_GT(node.getLevel(), -1);
        currentCount++;
    }
    ASSERT_EQ(expectedCount, currentCount);
}

TEST_F(DFTPreOrderTesting, ExpectedLevelOrderTest)
{
    ASSERT_TRUE(tree->hasNext());
    std::list<int> expectedLevelOrder = {0, 1, 2, 2, 2, 1, 1, 2, 2, 2};

    while (tree->hasNext()) {
        ASSERT_GT(expectedLevelOrder.size(), 0);
        auto node = tree->getNext();
        EXPECT_EQ(node.getLevel(), expectedLevelOrder.front());
        expectedLevelOrder.pop_front();
    }
    ASSERT_EQ(expectedLevelOrder.size(), 0);
}

TEST_F(DFTPostOrderTesting, RootAtEndTest)
{
    ASSERT_TRUE(tree->hasNext());
    while (tree->hasNext()) {
        auto node = tree->getNext();
        if (!tree->hasNext()) {
            ASSERT_EQ(node.getLevel(), rootLevel);
            ASSERT_EQ(&node.getItem(), getRoot());
        }
    }
}

TEST_F(DFTPostOrderTesting, SecondLevelChildAtFrontTest)
{
    ASSERT_TRUE(tree->hasNext());
    auto node = tree->getNext();
    ASSERT_EQ(node.getLevel(), secondLevel);
}

TEST_F(DFTPostOrderTesting, ExpectedNodesCountTest)
{
    const int expectedCount = 1 + get1stLevelChildren().size() + get2ndtLevelChildren().size();
    int currentCount        = 0;
    while (tree->hasNext()) {
        auto node = tree->getNext();
        EXPECT_GT(node.getLevel(), -1);
        currentCount++;
    }
    ASSERT_EQ(expectedCount, currentCount);
}

TEST_F(DFTPostOrderTesting, ExpectedLevelOrderTest)
{
    ASSERT_TRUE(tree->hasNext());
    std::list<int> expectedLevelOrder = {2, 2, 2, 1, 1, 2, 2, 2, 1, 0};

    while (tree->hasNext()) {
        ASSERT_GT(expectedLevelOrder.size(), 0);
        auto node = tree->getNext();
        EXPECT_EQ(node.getLevel(), expectedLevelOrder.front());
        expectedLevelOrder.pop_front();
    }
    ASSERT_EQ(expectedLevelOrder.size(), 0);
}
