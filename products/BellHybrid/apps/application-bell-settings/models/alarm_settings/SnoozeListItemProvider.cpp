// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include "SnoozeListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>

#include <apps-common/ApplicationCommon.hpp>
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

        auto onOff =
            new OnOffListItem(model.getSnoozeOnOff(), utils::translate("app_bell_settings_alarm_settings_snooze"));
        onOff->onProceed = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };
        internalData.emplace_back(onOff);
        constexpr auto snoozeLengthStep = 1U;
        constexpr auto snoozeLengthMin  = 1U;
        constexpr auto snoozeLengthMax  = 30U;
        internalData.emplace_back(
            new NumListItem(model.getSnoozeLength(),
                            UIntegerSpinner::Range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
                            utils::translate("app_bell_settings_alarm_settings_snooze_length")));

        const UTF8 minStr = utils::translate("common_minute_short");
        const auto range  = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{1, minStr},
            NumWithStringListItem::Value{2, minStr},
            NumWithStringListItem::Value{3, minStr},
            NumWithStringListItem::Value{5, minStr}};
        auto chimeInterval =
            new NumWithStringListItem(model.getSnoozeChimeInterval(),
                                      range,
                                      utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval"));
        chimeInterval->onProceed = [chimeInterval, this] {
            if (chimeInterval->isOff()) {
                this->onExit();
            }
        };
        internalData.emplace_back(chimeInterval);
        const auto textRange = UTF8Spinner::Range{{"Meditative\nsurprises"}}; // TODO: Full list needed
        internalData.emplace_back(
            new UTF8ListItem(model.getSnoozeChimeTone(),
                             textRange,
                             utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone")));
        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        internalData.emplace_back(
            new NumListItem(model.getSnoozeChimeVolume(),
                            UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                            utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume")));

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto SnoozeListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto SnoozeListItemProvider::getItem(Order order) -> ListItem *
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

    std::vector<BellSideListItemWithCallbacks *> SnoozeListItemProvider::getListItems()
    {
        return internalData;
    }

} // namespace app::bell_settings
