// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapMainWindowPresenter.hpp"
#include "models/PowerNapModel.hpp"
#include "data/PowerNapCommon.hpp"
#include <ApplicationBellPowerNap.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

namespace app::powernap
{
    PowerNapMainWindowPresenter::PowerNapMainWindowPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}, model{std::make_shared<PowerNapModel>()}
    {}

    auto PowerNapMainWindowPresenter::getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }

    void PowerNapMainWindowPresenter::loadNapTimeList()
    {
        const auto value = settings->getValue(powernapDBRecordName, settings::SettingsScope::AppLocal);
        model->createData();
        model->setValue(std::chrono::minutes{utils::getNumericValue<int>(value)});
    }

    void PowerNapMainWindowPresenter::activate()
    {
        const auto currentValue = model->getCurrentValue();
        settings->setValue(
            powernapDBRecordName, utils::to_string(currentValue.count()), settings::SettingsScope::AppLocal);
        reinterpret_cast<app::Application *>(app)->suspendIdleTimer();
        app->switchWindow(gui::window::name::powernapProgress);
    }
} // namespace app::powernap
