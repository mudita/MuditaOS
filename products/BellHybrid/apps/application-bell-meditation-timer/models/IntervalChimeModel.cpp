// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimeModel.hpp"

#include <ListView.hpp>

#include <i18n/i18n.hpp>

namespace
{
    constexpr std::chrono::minutes emptyValue{0};
} // namespace

namespace app::meditation
{
    auto IntervalChimeModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    auto IntervalChimeModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto IntervalChimeModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    void IntervalChimeModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    std::chrono::minutes IntervalChimeModel::getValue() const noexcept
    {
        if (item == nullptr) {
            return std::chrono::minutes{0};
        }
        return intervalToTime(IntervalType(item->getSpinnerIndex()));
    }

    void IntervalChimeModel::setValue(std::chrono::minutes value)
    {
        if (item == nullptr) {
            return;
        }
        IntervalType interval = timeToInterval(value);
        item->setSpinnerIndex(static_cast<int>(interval));
        if (list != nullptr) {
            list->rebuildList(gui::listview::RebuildType::Full);
        }
    }

    auto IntervalChimeModel::createData() -> void
    {
        auto onGetTitle = [this](int index) -> std::string {
            IntervalType interval = IntervalType(index);
            if (interval == IntervalType::IntervalNone) {
                return utils::translate("app_meditation_bell_interval_none");
            }

            const std::string toReplace = "%0";
            std::string temp            = utils::translate("app_meditation_bell_interval_every_x_minutes");

            switch (interval) {
            case IntervalType::IntervalNone:
            case IntervalType::IntervalTotal:
                temp = utils::translate("app_meditation_bell_interval_none");
                break;
            case IntervalType::Interval_1:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(1));
                break;
            case IntervalType::Interval_2:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(2));
                break;
            case IntervalType::Interval_5:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(5));
                break;
            case IntervalType::Interval_10:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(10));
                break;
            case IntervalType::Interval_15:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(15));
                break;
            case IntervalType::Interval_30:
                temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(30));
                break;
            }
            return temp;
        };

        std::vector<std::string> titles;
        for (int i = 0; i < static_cast<int>(IntervalType::IntervalTotal); i++) {
            titles.push_back(onGetTitle(i));
        }
        item = new gui::IntervalChimeListItem(titles);
        internalData.push_back(item);
        item->deleteByList = false;
        setValue(emptyValue);
    }

    IntervalChimeModel::IntervalType IntervalChimeModel::timeToInterval(std::chrono::minutes minutes) const noexcept
    {
        int s = minutes.count();
        if (s >= intervalToTime(IntervalType::Interval_30).count()) {
            return IntervalType::Interval_30;
        }
        if (s >= intervalToTime(IntervalType::Interval_15).count()) {
            return IntervalType::Interval_15;
        }
        if (s >= intervalToTime(IntervalType::Interval_10).count()) {
            return IntervalType::Interval_10;
        }
        if (s >= intervalToTime(IntervalType::Interval_5).count()) {
            return IntervalType::Interval_5;
        }
        if (s >= intervalToTime(IntervalType::Interval_2).count()) {
            return IntervalType::Interval_2;
        }
        if (s >= intervalToTime(IntervalType::Interval_1).count()) {
            return IntervalType::Interval_1;
        }
        return IntervalType::IntervalNone;
    }

    std::chrono::minutes IntervalChimeModel::intervalToTime(IntervalType interval) const noexcept
    {
        switch (interval) {
        case IntervalType::IntervalNone:
        case IntervalType::IntervalTotal:
            return std::chrono::minutes::zero();
        case IntervalType::Interval_1:
            return std::chrono::minutes{1};
        case IntervalType::Interval_2:
            return std::chrono::minutes{2};
        case IntervalType::Interval_5:
            return std::chrono::minutes{5};
        case IntervalType::Interval_10:
            return std::chrono::minutes{10};
        case IntervalType::Interval_15:
            return std::chrono::minutes{15};
        case IntervalType::Interval_30:
            return std::chrono::minutes{30};
        }
        return std::chrono::minutes::zero();
    }
} // namespace app::meditation
