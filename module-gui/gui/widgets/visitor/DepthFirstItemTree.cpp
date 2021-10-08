// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DepthFirstItemTree.hpp"

using namespace gui;

DepthFirstItemTree::DepthFirstItemTree(gui::Item &root, TraverseMode mode) : mode(mode)
{
    nodes.push(gui::ItemNode(root));
    if (mode == TraverseMode::PostOrder) {
        constructPostOrder(&root, ItemNode::rootLevel + 1);
    }
}

auto DepthFirstItemTree::hasNext() const noexcept -> bool
{
    return !nodes.empty();
}

auto DepthFirstItemTree::getNext() noexcept -> gui::ItemNode
{
    if (mode == TraverseMode::PostOrder) {
        return getNextInPostOrder();
    }
    return getNextInPreOrder();
}

void DepthFirstItemTree::constructPostOrder(gui::Item *item, int level)
{
    for (auto child = item->children.rbegin(); child != item->children.rend(); child++) {
        if (*child == nullptr) {
            continue;
        }
        nodes.push(gui::ItemNode(**child, level));
        constructPostOrder(*child, level + 1);
    }
}

auto DepthFirstItemTree::getNextInPostOrder() -> gui::ItemNode
{
    auto current = std::move(nodes.top());
    nodes.pop();
    return current;
}

auto DepthFirstItemTree::getNextInPreOrder() -> gui::ItemNode
{
    auto current   = std::move(nodes.top());
    auto &item     = current.getItem();
    auto nextLevel = current.getLevel() + 1;
    nodes.pop();
    for (auto child = item.children.rbegin(); child != item.children.rend(); child++) {
        nodes.push(gui::ItemNode(**child, nextLevel));
    }

    return current;
}
