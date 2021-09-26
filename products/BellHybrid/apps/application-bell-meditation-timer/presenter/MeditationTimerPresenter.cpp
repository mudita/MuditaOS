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
    {
        model->createData();
    }

    void MeditationTimerPresenter::activate(MeditationItem &item)
    {
        const auto value = settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal);
        int seconds      = utils::getNumericValue<int>(value);
        if (seconds >= std::chrono::seconds(app::meditation::value::minTimerValue).count() &&
            seconds <= std::chrono::seconds(app::meditation::value::maxTimerValue).count()) {
            item.setTimer(std::chrono::seconds{seconds});
        }

        model->setData(item);
        model->setOnTimerChanged([this]() { updateDisplay(); });
        updateDisplay();
    }

    void MeditationTimerPresenter::request(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }

    void MeditationTimerPresenter::increase()
    {
        model->increaseTimer();
    }

    void MeditationTimerPresenter::decrease()
    {
        model->decreaseTimer();
    }

    auto MeditationTimerPresenter::getTimerString() -> std::string
    {
        return model->getTimerString();
    }

    void MeditationTimerPresenter::updateDisplay()
    {
        getView()->updateDisplay();
    }
} // namespace app::meditation
