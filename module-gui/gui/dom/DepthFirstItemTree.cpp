// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DepthFirstItemTree.hpp"

using namespace gui;

DepthFirstItemTree::DepthFirstItemTree(gui::Item &root, TraverseMode mode) : mode(mode)
{
    nodes.push(gui::ItemNode(root, rootLevel));
    if (mode == TraverseMode::PostOrder) {
        constructPostOrder(&root, rootLevel + 1);
    }
}

auto DepthFirstItemTree::hasNode() const noexcept -> bool
{
    return !nodes.empty();
}

auto DepthFirstItemTree::getNext() noexcept -> gui::ItemNode
{
    if (mode == TraverseMode::PostOrder) {
        auto current = std::move(nodes.top());
        nodes.pop();
        return current;
    }
    return getNextInOrder();
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

auto DepthFirstItemTree::getNextInOrder() -> gui::ItemNode
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
