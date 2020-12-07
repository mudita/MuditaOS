// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ItemDomCreator.hpp"
#include "Item.hpp"
#include "Item2JsonSerializer.hpp"

using namespace gui;

ItemDomCreator::ItemDomCreator(DumpType type)
{
    switch (type) {
    case DumpType::JsonAtTraverseEnd:
        walker = std::make_unique<Item2JsonSerializer>();
        break;
    }
}

[[nodiscard]] auto ItemDomCreator::getWalker() noexcept -> std::unique_ptr<ItemWalker> &
{
    return walker;
}
