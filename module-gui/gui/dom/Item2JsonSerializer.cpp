// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Item2JsonSerializer.hpp"
#include "visitor/ItemNode.hpp"
#include "visitor/DepthFirstItemTree.hpp"
#include <fstream>
#include <module-utils/log/log.hpp>

#include <module-vfs/include/user/purefs/filesystem_paths.hpp>

using namespace gui;

namespace
{
    constexpr inline auto childrenName = "Children";
    constexpr inline auto fileName     = "itemDump.json";
}

void Item2JsonSerializer::handleSibling(gui::ItemNode &node, document &doc, int &level)
{
    node.getItem().accept(visitor);
    doc[level].emplace_back(json11::Json::object{{visitor.getName(), visitor.getState()}});
}

void Item2JsonSerializer::handleParent(gui::ItemNode &node, document &doc, int &level)
{
    auto parentLevel = node.getLevel();
    auto children    = json11::Json::array{};

    if (!doc[level].empty()) {
        while (!doc[level].empty()) {
            auto &&child = doc[level].front();
            children.emplace_back(std::move(child));
            doc[level].pop_front();
        }
    }
    doc[level].clear();
    level = parentLevel;

    node.getItem().accept(visitor);
    auto parent = visitor.getState();
    parent.emplace_back(json11::Json::object{{childrenName, std::move(children)}});
    doc[level].emplace_back(json11::Json::object{{visitor.getName(), std::move(parent)}});
}

void Item2JsonSerializer::handleOther(gui::ItemNode &node, document &doc, int &level)
{
    level      = node.getLevel();
    doc[level] = std::list<json11::Json::object>{};

    node.getItem().accept(visitor);
    doc[level].emplace_back(json11::Json::object{{visitor.getName(), visitor.getState()}});
}

void Item2JsonSerializer::traverse(gui::Item &root)
{
    int level = ItemNode::rootLevel;
    document documentPrototype;
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

    json11::Json document(std::move(documentPrototype[0].front()));
    dump(document);
}

void Item2JsonSerializer::dump(json11::Json &doc)
{
    std::string filename = purefs::dir::getUserDiskPath().append(::fileName);
    std::ofstream write;
    write.open(filename.c_str(), std::ios::out);
    if (!write.is_open()) {
        LOG_ERROR("Item serializer could not open file");
        return;
    }
    write << doc.dump();
    write.close();
}
