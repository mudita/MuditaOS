// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindowPresenter.hpp"

namespace app::bell_alarm
{
    BellAlarmWindowPresenter::BellAlarmWindowPresenter(std::unique_ptr<AbstractAlarmModel> &&alarmModel)
        : alarmModel{std::move(alarmModel)}
    {}

    auto BellAlarmWindowPresenter::saveData() -> void
    {
        auto view = getView();
        auto time = view->getAlarmTime();
        alarmModel->setAlarmTime(time);
    }

    auto BellAlarmWindowPresenter::createData() -> void
    {
        auto updateAlarmTimeCallback = [&]() {
            auto time = alarmModel->getAlarmTime();
            auto view = getView();
            view->setAlarmTime(time);
        };

        alarmModel->update(updateAlarmTimeCallback);
    }
} // namespace app::bell_alarm
