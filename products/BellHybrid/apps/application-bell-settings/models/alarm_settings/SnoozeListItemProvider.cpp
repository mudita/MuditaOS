// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "SnoozeListItemProvider.hpp"
#include "SnoozeListItems.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <common/models/SnoozeSettingsModel.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    SnoozeListItemProvider::SnoozeListItemProvider(AbstractSnoozeSettingsModel &model) : model{model}
    {
        buildListItems();
    }

    void SnoozeListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 5U;
        internalData.reserve(itemCount);

        auto onOff    = new SnoozeOnOffListItem(model);
        onOff->onExit = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };
        internalData.emplace_back(onOff);
        internalData.emplace_back(new SnoozeLengthListItem(model));

        auto chimeInterval    = new SnoozeChimeIntervalListItem(model);
        chimeInterval->onExit = [chimeInterval, this] {
            if (chimeInterval->isOff()) {
                this->onExit();
            }
        };
        internalData.emplace_back(chimeInterval);
        internalData.emplace_back(new SnoozeChimeToneListItem(model));
        internalData.emplace_back(new SnoozeChimeVolumeListItem(model));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto SnoozeListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto SnoozeListItemProvider::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    auto SnoozeListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto SnoozeListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    std::vector<gui::BellSideListItemWithCallbacks *> SnoozeListItemProvider::getListItems()
    {
        return internalData;
    }

} // namespace app::bell_settings
