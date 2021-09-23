// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimerPresenter.hpp"

#include "log.hpp"
#include <gsl/assert>

namespace app::meditation
{
    MeditationTimerPresenter ::MeditationTimerPresenter(app::ApplicationCommon *app)
        : app{app}, model{std::make_shared<MeditationTimerModel>()}
    {
        model->createData();
    }

    void MeditationTimerPresenter::activate(MeditationItem &item)
    {
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
