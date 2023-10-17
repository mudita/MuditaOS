// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsListItemProvider.hpp"
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{

    std::vector<gui::BellSideListItemWithCallbacks *> SettingsListItemProvider::getListItems()
    {
        return internalData;
    }
    auto SettingsListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }
    auto SettingsListItemProvider::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }
    auto SettingsListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }
    auto SettingsListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }
    void SettingsListItemProvider::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::bell_settings
