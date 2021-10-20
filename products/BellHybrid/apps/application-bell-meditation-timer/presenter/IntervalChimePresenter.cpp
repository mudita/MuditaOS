// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "IntervalChimePresenter.hpp"
#include "MeditationCommon.hpp"

#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

namespace app::meditation
{
    IntervalChimePresenter ::IntervalChimePresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}, model{std::make_shared<IntervalChimeModel>()}
    {}

    auto IntervalChimePresenter::getProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }

    void IntervalChimePresenter::loadIntervalList()
    {
        model->createData();
    }

    void IntervalChimePresenter::activate()
    {
        const auto value = model->getValue();
        settings->setValue(intervalDBRecordName, utils::to_string(value.count()), settings::SettingsScope::AppLocal);
        app->switchWindow(gui::name::window::readyGoing);
    }
} // namespace app::meditation
