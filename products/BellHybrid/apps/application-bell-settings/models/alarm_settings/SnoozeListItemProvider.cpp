// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozeListItemProvider.hpp"
#include <common/widgets/ListItems.hpp>
#include <common/LanguageUtils.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    NumWithStringListItem::NumWithStringSpinner::Range getDefaultChimeIntervalRange()
    {
        const UTF8 minStr = utils::translate("common_minute_short");
        return {
            NumWithStringListItem::Value{utils::translate("app_settings_toggle_off")},
            NumWithStringListItem::Value{1, minStr},
            NumWithStringListItem::Value{2, minStr},
            NumWithStringListItem::Value{3, minStr},
            NumWithStringListItem::Value{5, minStr},
        };
    }

    NumWithStringListItem::NumWithStringSpinner::Range transformChimeIntervalsRange(const std::uint32_t chimeLength)
    {
        auto chimeIntervals = getDefaultChimeIntervalRange();

        chimeIntervals.erase(std::remove_if(chimeIntervals.begin(),
                                            chimeIntervals.end(),
                                            [chimeLength](const auto &e) {
                                                return e.getValue() && (e.getValue().value() >= chimeLength);
                                            }),
                             chimeIntervals.end());

        return chimeIntervals;
    }

    std::optional<NumWithStringListItem::Value> calculateCurrentChimeIntervalValue(
        const NumWithStringListItem::NumWithStringSpinner::Range &range,
        const NumWithStringListItem::Value &chimeInterval)
    {
        if (range.size() == 1) {
            return {};
        }

        if (chimeInterval.getValue() && (chimeInterval.getValue().value() >= range.back().getValue().value())) {
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
        buildListItems(chimeTonesRange);
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
        ;
        auto chimeLength = new NumListItem(model.getSnoozeLength(),
                                           UIntegerSpinner::Range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
                                           utils::translate("app_bell_settings_alarm_settings_snooze_length"),
                                           chimeLengthBottomDescription);

        chimeLength->setOnValueChanged([chimeLength](const std::uint32_t &val) {
            chimeLength->setBottomDescribtionText(utils::language::getCorrectMinutesNumeralForm(val));
        });

        chimeLength->onEnter = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };

        internalData.emplace_back(chimeLength);

        auto chimeInterval = new NumWithStringListItem(
            model.getSnoozeChimeInterval(),
            getDefaultChimeIntervalRange(),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval"),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval_bot_desc"));
        chimeLength->setValue();

        internalData.emplace_back(chimeInterval);

        chimeInterval->onEnter = [chimeInterval, chimeLength, this]() {
            if (chimeLength != nullptr) {
                const auto currentChimeLength   = chimeLength->getCurrentValue();
                const auto currentChimeInterval = chimeInterval->getCurrentValue();
                const auto calculatedRange      = transformChimeIntervalsRange(currentChimeLength);
                const auto calculatedChimeInterval =
                    calculateCurrentChimeIntervalValue(calculatedRange, currentChimeInterval);

                if (calculatedChimeInterval) {
                    chimeInterval->getSpinner()->setRange(calculatedRange);
                    chimeInterval->getSpinner()->setCurrentValue(calculatedChimeInterval.value());
                    chimeInterval->setArrowsVisibility();
                }
                else {
                    this->onExit();
                }
            }
            return false;
        };

        auto snoozeChimeTone = new UTF8ListItem(model.getSnoozeChimeTone(),
                                                std::move(chimeTonesRange),
                                                utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone"));
        snoozeChimeTone->setOnValueChanged([this](const UTF8 &val) {
            if (onToneChange) {
                onToneChange(val);
            }
        });
        snoozeChimeTone->onEnter = [this, snoozeChimeTone, chimeInterval]() {
            if (chimeInterval->isOff()) {
                this->onExit();
                return;
            }
            if (onToneEnter) {
                onToneEnter(snoozeChimeTone->getCurrentValue());
            }
        };
        snoozeChimeTone->onExit = [this, snoozeChimeTone]() {
            if (onToneExit) {
                onToneExit(snoozeChimeTone->getCurrentValue());
            }
        };
        internalData.emplace_back(snoozeChimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = 1U;
        constexpr auto volumeMax  = 10U;
        auto snoozeChimeVolume =
            new NumListItem(model.getSnoozeChimeVolume(),
                            UIntegerSpinner::Range{volumeMin, volumeMax, volumeStep},
                            utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume"));
        snoozeChimeVolume->setOnValueChanged([this](const UIntegerSpinner::Type &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        snoozeChimeVolume->onEnter = [this, snoozeChimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(snoozeChimeTone->getCurrentValue());
            }
        };

        snoozeChimeVolume->onExit = [this, snoozeChimeVolume]() {
            if (onVolumeExit) {
                onVolumeExit(snoozeChimeVolume->getCurrentValue());
            }
        };

        internalData.emplace_back(snoozeChimeVolume);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

} // namespace app::bell_settings
