// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsStyle.hpp"
#include <common/widgets/ListItems.hpp>
#include "PrewakeUpListItemProvider.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    PrewakeUpListItemProvider::PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &model) : model{model}
    {
        buildListItems();
    }

    void PrewakeUpListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 4U;
        internalData.reserve(itemCount);

        const UTF8 minStr             = utils::translate("common_minute_short");
        const auto chimeDurationRange = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{5, minStr},
            NumWithStringListItem::Value{10, minStr},
            NumWithStringListItem::Value{15, minStr}};
        auto chimeDuration = new NumWithStringListItem(
            model.getChimeDuration(),
            chimeDurationRange,
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_bottom_description"));
        chimeDuration->onExit = [chimeDuration, this] {
            if (chimeDuration->isOff()) {
                constexpr auto lightDurationListIndex = 3U;
                list->rebuildList(gui::listview::RebuildType::OnOffset, lightDurationListIndex);
            }
        };
        internalData.emplace_back(chimeDuration);

        const auto textRange = UTF8Spinner::Range{{"Meditative\nsurprises"}}; // TODO: Full list needed
        internalData.emplace_back(
            new UTF8ListItem(model.getChimeTone(),
                             textRange,
                             utils::translate("app_bell_settings_alarm_settings_prewake_up_chime_tone")));

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        internalData.emplace_back(
            new NumListItem(model.getChimeVolume(),
                            UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                            utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume")));

        const auto lightDurationRange = NumWithStringListItem::NumWithStringSpinner::Range{
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{5, minStr},
            NumWithStringListItem::Value{10, minStr},
            NumWithStringListItem::Value{15, minStr}};
        auto lightDuration = new NumWithStringListItem(
            model.getLightDuration(),
            lightDurationRange,
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_top_description"),
            utils::translate("app_bell_settings_alarm_settings_prewake_up_light_bottom_description"));
        lightDuration->onExit = [lightDuration, this] {
            if (lightDuration->isOff()) {
                this->onExit();
            }
        };
        internalData.emplace_back(lightDuration);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto PrewakeUpListItemProvider::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto PrewakeUpListItemProvider::getItem(Order order) -> ListItem *
    {
        return getRecord(order);
    }

    auto PrewakeUpListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto PrewakeUpListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    std::vector<BellSideListItemWithCallbacks *> PrewakeUpListItemProvider::getListItems()
    {
        return internalData;
    }

    void PrewakeUpListItemProvider::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::bell_settings
