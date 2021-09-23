// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeListItemProvider.hpp"
#include "common/widgets/ListItems.hpp"
#include "data/BellBedtimeStyle.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <service-time/api/TimeSettingsApi.hpp>

namespace app::bell_bedtime
{
    using namespace gui;

    BedtimeListItemProvider::BedtimeListItemProvider(std::unique_ptr<AbstractBedtimeModel> &&model)
        : model{std::move(model)}
    {
        buildListItems();
    }

    void BedtimeListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 2U;
        internalData.reserve(itemCount);

        auto onOff       = new OnOffListItem(model.get()->getBedtimeOnOff(), utils::translate("app_bellmain_bedtime"));
        onOff->onProceed = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit(false);
            }
        };
        internalData.emplace_back(onOff);
        internalData.emplace_back(new TimeListItem(model.get()->getBedtimeTime(),
                                                   stm::api::timeFormat(),
                                                   bell_bedtime_style::time_set_fmt_spinner::focusFont,
                                                   bell_bedtime_style::time_set_fmt_spinner::noFocusFont,
                                                   utils::translate("app_bellmain_bedtime")));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto BedtimeListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto BedtimeListItemProvider::getItem(Order order) -> ListItem *
    {
        return getRecord(order);
    }

    auto BedtimeListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto BedtimeListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto BedtimeListItemProvider::getListItems() -> std::vector<BellSideListItemWithCallbacks *>
    {
        return internalData;
    }

} // namespace app::bell_bedtime
