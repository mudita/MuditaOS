// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationTimerPresenter.hpp"
#include "MeditationCommon.hpp"

#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

namespace app::meditation
{
    MeditationTimerPresenter ::MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}, model{std::make_shared<MeditationTimerModel>()}
    {}

    auto MeditationTimerPresenter::getProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }

    void MeditationTimerPresenter::loadTimerList()
    {
        const auto value = settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal);
        model->createData();
        model->setValue(std::chrono::minutes{utils::getNumericValue<int>(value)});
    }

    void MeditationTimerPresenter::activate()
    {
        const auto value = model->getValue();
        settings->setValue(meditationDBRecordName, utils::to_string(value.count()), settings::SettingsScope::AppLocal);
        app->switchWindow(gui::name::window::intervalChime);
    }
} // namespace app::meditation
