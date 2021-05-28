// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Item2JsonSerializingVisitor.hpp"
#include <json11.hpp>
#include <list>

namespace gui
{
    class ItemNode;

    class Item2JsonSerializer
    {
        using prototype = std::map<int, std::list<json11::Json::object>>;
        json11::Json document;
        Item2JsonSerializingVisitor visitor;

        void handleSibling(gui::ItemNode &node, prototype &doc, int &level);
        void handleParent(gui::ItemNode &node, prototype &doc, int &level);
        void handleOther(gui::ItemNode &node, prototype &doc, int &level);

      public:
        /// On traverse all `gui::Item`-based classes in the parent-children tree of given root will be serialized into
        /// JSON format and kept in `document`.
        void traverse(gui::Item &root);
        void dump(std::ostream &stream);
        [[nodiscard]] auto get() -> json11::Json &&;
    };
} // namespace gui
