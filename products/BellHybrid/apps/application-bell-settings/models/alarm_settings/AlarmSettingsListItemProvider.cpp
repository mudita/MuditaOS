// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include <common/widgets/ListItems.hpp>
#include "AlarmSettingsListItemProvider.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    AlarmSettingsListItemProvider::AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &model) : model{model}
    {
        buildListItems();
    }

    void AlarmSettingsListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 4U;
        internalData.reserve(itemCount);

        const auto textRange = UTF8Spinner::Range{{"Meditative\nsurprises"}};
        internalData.emplace_back(new UTF8ListItem(
            model.getAlarmTone(), textRange, utils::translate("app_bell_settings_alarm_settings_tone")));

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        internalData.emplace_back(new NumListItem(model.getAlarmVolume(),
                                                  UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                                                  utils::translate("app_bell_settings_alarm_settings_volume")));

        internalData.emplace_back(
            new OnOffListItem(model.getAlarmLightOnOff(), utils::translate("app_bell_settings_alarm_settings_light")));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto AlarmSettingsListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto AlarmSettingsListItemProvider::getItem(Order order) -> ListItem *
    {
        return getRecord(order);
    }

    auto AlarmSettingsListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto AlarmSettingsListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    std::vector<BellSideListItemWithCallbacks *> AlarmSettingsListItemProvider::getListItems()
    {
        return internalData;
    }

    void AlarmSettingsListItemProvider::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::bell_settings
