// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindowPresenter.hpp"

namespace app::bell_alarm
{
    BellAlarmWindowPresenter::BellAlarmWindowPresenter(AbstractAlarmModel &alarmModel,
                                                       std::shared_ptr<AbstractTimeModel> timeModel)
        : alarmModel{alarmModel}, timeModel{std::move(timeModel)}
    {}

    auto BellAlarmWindowPresenter::onBeforeShow() -> void
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    auto BellAlarmWindowPresenter::saveData() -> void
    {
        auto view       = getView();
        const auto time = view->getAlarmTime();
        alarmModel.setAlarmTime(time);
    }

    auto BellAlarmWindowPresenter::createData() -> void
    {
        auto updateAlarmTimeCallback = [&]() {
            const auto time = alarmModel.getAlarmTime();
            auto view       = getView();
            view->setAlarmTime(time);
        };

        alarmModel.update(updateAlarmTimeCallback);
    }

    auto BellAlarmWindowPresenter::setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void
    {
        auto view = getView();
        view->setTimeFormat(fmt);
    }

} // namespace app::bell_alarm
