// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationStyle.hpp"
#include "MeditationTimerPresenter.hpp"

#include <service-db/Settings.hpp>

namespace
{
    constexpr inline auto spinnerMax   = 180U;
    constexpr inline auto spinnerMin   = 1U;
    constexpr inline auto spinnerStep  = 1U;
    constexpr inline auto emptyValue   = 0U;
    constexpr inline auto defaultValue = 15U;
} // namespace

namespace app::meditation
{
    MeditationTimerPresenter ::MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}
    {}

    std::uint32_t MeditationTimerPresenter::getMinValue()
    {
        return spinnerMin;
    }

    std::uint32_t MeditationTimerPresenter::getMaxValue()
    {
        return spinnerMax;
    }

    std::uint32_t MeditationTimerPresenter::getStepValue()
    {
        return spinnerStep;
    }

    std::uint32_t MeditationTimerPresenter::getCurrentValue()
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
        using namespace app::meditationStyle::mtStyle::list;
        return utils::translate(value == 1 ? timeUnitSingular : timeUnitPlural);
    }

    void MeditationTimerPresenter::activate(std::uint32_t value)
    {
        settings->setValue(meditationDBRecordName, utils::to_string(value), settings::SettingsScope::AppLocal);
        app->switchWindow(gui::name::window::intervalChime);
    }
} // namespace app::meditation
