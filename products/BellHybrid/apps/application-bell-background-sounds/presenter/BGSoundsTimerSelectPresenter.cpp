// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsTimerSelectPresenter.hpp"
#include "data/BGSoundsCommon.hpp"

#include <ApplicationBellBackgroundSounds.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

namespace
{
    using namespace std::chrono_literals;
    constexpr auto timerDefaultValue{15min};
    const static std::vector<std::chrono::minutes> valuesRange{0min, 5min, 10min, 15min, 30min, 45min, 60min, 90min};
} // namespace

namespace app::bgSounds
{
    BGSoundsTimerSelectPresenter::BGSoundsTimerSelectPresenter(settings::Settings *settings) : settings{settings}
    {}
    const BGSoundsTimerSelectContract::Range &BGSoundsTimerSelectPresenter::getTimerValuesRange() const noexcept
    {
        return valuesRange;
    }
    std::chrono::minutes BGSoundsTimerSelectPresenter::getCurrentTimerValue() const
    {
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        if (not value.empty()) {
            return std::chrono::minutes{utils::getNumericValue<int>(value)};
        }
        return timerDefaultValue;
    }
    void BGSoundsTimerSelectPresenter::setTimerValue(std::chrono::minutes value)
    {
        settings->setValue(timerValueDBRecordName, utils::to_string(value.count()), settings::SettingsScope::AppLocal);
    }
} // namespace app::bgSounds
