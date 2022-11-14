// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationTimerSelectPresenter.hpp"
#include <data/RelaxationCommon.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <service-db/Settings.hpp>

namespace
{
    using namespace std::chrono_literals;
    using minutes = std::chrono::minutes;
    constexpr auto timerDefaultValue{15min};
    const static std::vector<minutes> valuesRange{
        minutes::zero(), 5min, 10min, 15min, 30min, 45min, 60min, 90min, 8760h};
} // namespace

namespace app::relaxation
{
    RelaxationTimerSelectPresenter::RelaxationTimerSelectPresenter(settings::Settings *settings) : settings{settings}
    {}

    const RelaxationTimerSelectContract::Range &RelaxationTimerSelectPresenter::getTimerValuesRange() const noexcept
    {
        return valuesRange;
    }

    std::chrono::minutes RelaxationTimerSelectPresenter::getCurrentTimerValue() const
    {
        const auto value = settings->getValue(timerValueDBRecordName, settings::SettingsScope::AppLocal);
        if (not value.empty()) {
            return std::chrono::minutes{utils::getNumericValue<int>(value)};
        }
        return timerDefaultValue;
    }

    void RelaxationTimerSelectPresenter::setTimerValue(std::chrono::minutes value)
    {
        settings->setValue(timerValueDBRecordName, utils::to_string(value.count()), settings::SettingsScope::AppLocal);
    }
} // namespace app::relaxation
