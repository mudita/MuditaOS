// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozeListItemProvider.hpp"
#include <common/widgets/list_items/NumberWithSuffix.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <common/widgets/ListItems.hpp>
#include <common/LanguageUtils.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    list_items::NumberWithSuffix::spinner_type::range getDefaultChimeIntervalRange()
    {
        return {0, 1, 2, 3, 5};
    }

    list_items::NumberWithSuffix::spinner_type::range transformChimeIntervalsRange(const std::uint32_t chimeLength)
    {
        auto chimeIntervals = getDefaultChimeIntervalRange();

        chimeIntervals.erase(std::remove_if(chimeIntervals.begin(),
                                            chimeIntervals.end(),
                                            [chimeLength](const auto &e) {
                                                if (e == chimeLength) {
                                                    return true;
                                                }
                                                return e and ((chimeLength % e) != 0);
                                            }),
                             chimeIntervals.end());

        return chimeIntervals;
    }

    std::optional<list_items::NumberWithSuffix::spinner_type::value_type> calculateCurrentChimeIntervalValue(
        const list_items::NumberWithSuffix::spinner_type::range &range,
        const list_items::NumberWithSuffix::spinner_type::value_type &chimeInterval)
    {
        if (range.size() == 1) {
            return {};
        }

        if (chimeInterval >= range.back()) {
            return range.back();
        }
        else {
            return chimeInterval;
        }
    }

    SnoozeListItemProvider::SnoozeListItemProvider(AbstractSnoozeSettingsModel &model,
                                                   std::vector<UTF8> chimeTonesRange)
        : model{model}
    {
        buildListItems(std::move(chimeTonesRange));
    }

    void SnoozeListItemProvider::buildListItems(std::vector<UTF8> chimeTonesRange)
    {
        constexpr auto itemCount = 5U;
        internalData.reserve(itemCount);

        auto onOff =
            new OnOffListItem(model.getSnoozeOnOff(), utils::translate("app_bell_settings_alarm_settings_snooze"));

        internalData.emplace_back(onOff);
        constexpr auto snoozeLengthStep = 1U;
        constexpr auto snoozeLengthMin  = 1U;
        constexpr auto snoozeLengthMax  = 30U;

        auto chimeLengthBottomDescription =
            utils::language::getCorrectMinutesNumeralForm(model.getSnoozeLength().getValue());
        auto chimeLength = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
            model.getSnoozeLength(),
            utils::translate("app_bell_settings_alarm_settings_snooze_length"),
            chimeLengthBottomDescription);

        chimeLength->set_on_value_change_cb([chimeLength](const auto &val) {
            chimeLength->setBottomDescribtionText(utils::language::getCorrectMinutesNumeralForm(val));
        });

        chimeLength->onEnter = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };

        internalData.emplace_back(chimeLength);

        auto chimeInterval = new list_items::NumberWithSuffix(
            getDefaultChimeIntervalRange(),
            model.getSnoozeChimeInterval(),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval"),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval_bot_desc"));
        chimeLength->setValue();

        internalData.emplace_back(chimeInterval);

        chimeInterval->onEnter = [chimeInterval, chimeLength, this]() {
            if (chimeLength != nullptr) {
                const auto currentChimeLength   = chimeLength->value();
                const auto currentChimeInterval = chimeInterval->value();
                const auto calculatedRange      = transformChimeIntervalsRange(currentChimeLength);
                const auto calculatedChimeInterval =
                    calculateCurrentChimeIntervalValue(calculatedRange, currentChimeInterval);

                if (calculatedChimeInterval) {
                    chimeInterval->set_range(calculatedRange);
                    chimeInterval->set_value(calculatedChimeInterval.value());
                }
                else {
                    this->onExit();
                }
            }
            return false;
        };

        auto snoozeChimeTone =
            new list_items::Text(std::move(chimeTonesRange),
                                 model.getSnoozeChimeTone(),
                                 utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone"));
        snoozeChimeTone->set_on_value_change_cb([this](const auto &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        snoozeChimeTone->onEnter = [this, snoozeChimeTone, chimeInterval]() {
            if (chimeInterval->value() == 0) {
                this->onExit();
                return;
            }
            if (onToneEnter) {
                onToneEnter(snoozeChimeTone->value());
            }
        };
        snoozeChimeTone->onExit = [this, snoozeChimeTone]() {
            if (onToneExit) {
                onToneExit(snoozeChimeTone->value());
            }
        };
        internalData.emplace_back(snoozeChimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto snoozeChimeVolume =
            new list_items::Numeric(list_items::Numeric::spinner_type::range{volumeMin, volumeMax, volumeStep},
                                    model.getSnoozeChimeVolume(),
                                    utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume"));
        snoozeChimeVolume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        snoozeChimeVolume->onEnter = [this, snoozeChimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(snoozeChimeTone->value());
            }
        };

        snoozeChimeVolume->onExit = [this, snoozeChimeVolume]() {
            if (onVolumeExit) {
                onVolumeExit(snoozeChimeVolume->value());
            }
        };

        internalData.emplace_back(snoozeChimeVolume);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

} // namespace app::bell_settings
