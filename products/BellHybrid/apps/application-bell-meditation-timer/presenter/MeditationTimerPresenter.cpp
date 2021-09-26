// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerPresenter.hpp"
#include "MeditationCommon.hpp"

#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/Settings.hpp>

namespace app::meditation
{
    MeditationTimerPresenter ::MeditationTimerPresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}, model{std::make_shared<MeditationTimerModel>()}
    {}

    void MeditationTimerPresenter::set(MeditationItem &item)
    {
        const auto value = settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal);
        int seconds      = utils::getNumericValue<int>(value);
        if (seconds >= std::chrono::seconds(app::meditation::value::minTimerValue).count() &&
            seconds <= std::chrono::seconds(app::meditation::value::maxTimerValue).count()) {
            item.setTimer(std::chrono::seconds{seconds});
        }

        model->createData();
        model->setData(item);
    }

    void MeditationTimerPresenter::get(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }

    auto MeditationTimerPresenter::getProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }
} // namespace app::meditation
