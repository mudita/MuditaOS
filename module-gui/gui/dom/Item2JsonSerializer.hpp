// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "visitor/ItemWalker.hpp"
#include "Item2JsonSerializingVisitor.hpp"
#include <module-utils/json/json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;

    class Item2JsonSerializer : public ItemWalker
    {
        using document = std::map<int, std::list<json11::Json::object>>;

        Item2JsonSerializingVisitor visitor;

        void handleSibling(gui::ItemNode &node, document &doc, int &level);
        void handleParent(gui::ItemNode &node, document &doc, int &level);
        void handleOther(gui::ItemNode &node, document &doc, int &level);
        void dump(json11::Json &doc);

      public:
        void traverse(gui::Item &root) override;
    };
} // namespace gui
