// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapModel.hpp"
#include "data/PowerNapListItem.hpp"
#include <ListView.hpp>
#include <gui/widgets/Style.hpp>

namespace
{
    constexpr std::chrono::minutes emptyValue{0};
    constexpr std::chrono::minutes spinnerDefaultValue{15};
} // namespace

namespace app::powernap
{
    auto PowerNapModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }
    auto PowerNapModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }
    auto PowerNapModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }
    void PowerNapModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    std::chrono::minutes PowerNapModel::getCurrentValue() const noexcept
    {
        if (powerNapItem == nullptr) {
            return emptyValue;
        }
        return std::chrono::minutes{powerNapItem->getSpinnerValue()};
    }
    void PowerNapModel::setValue(std::chrono::minutes napTimeValue)
    {
        if (powerNapItem == nullptr) {
            return;
        }
        if (napTimeValue == emptyValue) {
            napTimeValue = spinnerDefaultValue;
        }
        powerNapItem->setSpinnerValue(napTimeValue.count());
        if (list != nullptr) {
            list->rebuildList(gui::listview::RebuildType::Full);
        }
    }

    auto PowerNapModel::createData() -> void
    {
        powerNapItem = new gui::PowerNapListItem();
        internalData.push_back(powerNapItem);
        powerNapItem->deleteByList = false;
    }
} // namespace app::powernap
