// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Item2JsonSerializer.hpp"
#include "visitor/ItemNode.hpp"
#include "visitor/DepthFirstItemTree.hpp"
#include "ItemDataNames.hpp"

#include <magic_enum.hpp>
using namespace gui;

void Item2JsonSerializer::handleSibling(gui::ItemNode &node, prototype &doc, int &level)
{
    node.getItem().accept(visitor);
    doc[level].emplace_back(json11::Json::object{{visitor.moveName(), visitor.moveState()}});
}

void Item2JsonSerializer::handleParent(gui::ItemNode &node, prototype &doc, int &level)
{
    auto parentLevel = node.getLevel();
    auto children    = json11::Json::array{};

    while (!doc[level].empty()) {
        auto &&child = doc[level].front();
        children.emplace_back(std::move(child));
        doc[level].pop_front();
    }

    doc[level].clear();
    level = parentLevel;

    node.getItem().accept(visitor);
    auto parent = visitor.moveState();
    parent.emplace(magic_enum::enum_name(visitor::Names::Children), std::move(children));
    doc[level].emplace_back(json11::Json::object{{visitor.moveName(), std::move(parent)}});
}

void Item2JsonSerializer::handleOther(gui::ItemNode &node, prototype &doc, int &level)
{
    level      = node.getLevel();
    doc[level] = std::list<json11::Json::object>{};

    node.getItem().accept(visitor);
    doc[level].emplace_back(json11::Json::object{{visitor.moveName(), visitor.moveState()}});
}

void Item2JsonSerializer::traverse(gui::Item &root)
{
    int level = ItemNode::rootLevel;
    prototype documentPrototype;
    DepthFirstItemTree nodes(root, DepthFirstItemTree::TraverseMode::PostOrder);

    while (nodes.hasNext()) {
        auto node = nodes.getNext();
        if (node.getLevel() == level) {
            handleSibling(node, documentPrototype, level);
        }
        else if (node.getLevel() < level) {
            handleParent(node, documentPrototype, level);
        }
        else {
            handleOther(node, documentPrototype, level);
        }
    }
    document = json11::Json(std::move(documentPrototype[0].front()));
}

void Item2JsonSerializer::dump(std::ostream &stream)
{
    stream << document.dump();
}

[[nodiscard]] auto Item2JsonSerializer::get() -> json11::Json &&
{
    return std::move(document);
}
