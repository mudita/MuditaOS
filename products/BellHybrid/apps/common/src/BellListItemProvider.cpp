// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellListItemProvider.hpp"
#include <gui/widgets/ListViewEngine.hpp>

namespace app
{

    std::vector<gui::BellSideListItemWithCallbacks *> BellListItemProvider::getListItems()
    {
        return internalData;
    }
    auto BellListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }
    auto BellListItemProvider::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }
    auto BellListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }
    auto BellListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }
    void BellListItemProvider::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app
