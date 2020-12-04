// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ItemDomCreator.hpp"
#include "Item.hpp"
#include "Item2JsonSerializingVisitor.hpp"
#include "Item2PrettyPrintSerializingVisitor.hpp"

using namespace gui;

ItemDomCreator::ItemDomCreator(DumpType type)
{
    switch (type) {
    case DumpType::JsonAtTraverseEnd:
        walker = std::make_unique<Item2JsonSerializingVisitor>();
        break;
    case DumpType::PrettyPrintAtNodeEnd:
        walker = std::make_unique<Item2PrettyPrintSerializingVisitor>();
        break;
    }
}

void ItemDomCreator::traverse(gui::Item &root)
{
    walker->traverse(root);
}
