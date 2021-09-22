// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumeModel.hpp"
#include "data/BGSoundsVolumeListItem.hpp"
#include <ListView.hpp>
#include <gui/widgets/Style.hpp>

namespace
{
    constexpr auto emptyValue{0};
    constexpr auto spinnerDefaultValue{5};
} // namespace

namespace app::bgSounds
{
    auto BGSoundsVolumeModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }
    auto BGSoundsVolumeModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }
    auto BGSoundsVolumeModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }
    void BGSoundsVolumeModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    int BGSoundsVolumeModel::getCurrentValue() const noexcept
    {
        if (volumeItem == nullptr) {
            return emptyValue;
        }
        return volumeItem->getSpinnerValue();
    }
    void BGSoundsVolumeModel::setValue(int value)
    {
        if (volumeItem == nullptr) {
            return;
        }
        if (value == emptyValue) {
            value = spinnerDefaultValue;
        }
        volumeItem->setSpinnerValue(value);
        if (list != nullptr) {
            list->rebuildList(gui::listview::RebuildType::Full);
        }
    }

    auto BGSoundsVolumeModel::createData() -> void
    {
        volumeItem = new gui::BGSoundsVolumeListItem();
        internalData.push_back(volumeItem);
        volumeItem->deleteByList = false;
        setValue(spinnerDefaultValue);
    }
} // namespace app::bgSounds
