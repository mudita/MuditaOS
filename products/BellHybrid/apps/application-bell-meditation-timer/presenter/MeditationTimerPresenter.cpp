// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationCommon.hpp"
#include "MeditationTimerPresenter.hpp"

#include <common/LanguageUtils.hpp>
#include <service-db/Settings.hpp>

namespace
{
    constexpr auto spinnerMax   = 180U;
    constexpr auto spinnerMin   = 1U;
    constexpr auto spinnerStep  = 1U;
    constexpr auto emptyValue   = 0U;
    constexpr auto defaultValue = 15U;
} // namespace

namespace app::meditation
{
    MeditationTimerPresenter::MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}
    {}

    std::uint8_t MeditationTimerPresenter::getMinValue()
    {
        return spinnerMin;
    }

    std::uint8_t MeditationTimerPresenter::getMaxValue()
    {
        return spinnerMax;
    }

    std::uint8_t MeditationTimerPresenter::getStepValue()
    {
        return spinnerStep;
    }

    std::uint8_t MeditationTimerPresenter::getCurrentValue()
    {
        const auto value = settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal);
        auto defTimer    = utils::getNumericValue<std::uint32_t>(value);
        if (defTimer == emptyValue) {
            defTimer = defaultValue;
        }
        return defTimer;
    }

    std::string MeditationTimerPresenter::getTimeUnitName(std::uint32_t value)
    {
        return utils::language::getCorrectMinutesNumeralForm(value);
    }

    void MeditationTimerPresenter::activate(std::uint32_t value)
    {
        settings->setValue(meditationDBRecordName, utils::to_string(value), settings::SettingsScope::AppLocal);
        app->switchWindow(windows::meditationCountdown);
    }
} // namespace app::meditation
