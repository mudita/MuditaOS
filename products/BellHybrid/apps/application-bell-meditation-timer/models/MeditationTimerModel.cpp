// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerModel.hpp"

#include <ListView.hpp>
#include <i18n/i18n.hpp>

namespace
{
    constexpr std::chrono::minutes emptyValue{0};
    constexpr std::chrono::minutes spinnerDefaultValue{15};
} // namespace

namespace app::meditation
{
    auto MeditationTimerModel::createData() -> void
    {
        MeditationBaseModel::createData();

        item = new gui::MeditationTimerListItem();
        internalData.push_back(item);
        item->deleteByList = false;
        item->setOnValueChanged([this](int value) {
            meditationItem->setTimer(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::minutes{value}));
        });
    }

    auto MeditationTimerModel::setData(MeditationItem &item) -> void
    {
        MeditationBaseModel::setData(item);

        setValue(std::chrono::duration_cast<std::chrono::minutes>(item.getTimer()));
    }

    auto MeditationTimerModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    auto MeditationTimerModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto MeditationTimerModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    void MeditationTimerModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    std::chrono::minutes MeditationTimerModel::getValue() const noexcept
    {
        if (item == nullptr) {
            return emptyValue;
        }
        return std::chrono::minutes{item->getSpinnerValue()};
    }

    void MeditationTimerModel::setValue(std::chrono::minutes value)
    {
        if (item == nullptr) {
            return;
        }
        if (value == emptyValue) {
            value = spinnerDefaultValue;
        }
        item->setSpinnerValue(value.count());
        if (list != nullptr) {
            list->rebuildList(gui::listview::RebuildType::Full);
        }
    }
} // namespace app::meditation
