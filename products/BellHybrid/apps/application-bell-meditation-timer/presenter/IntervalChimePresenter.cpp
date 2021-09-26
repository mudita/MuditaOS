// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IntervalChimePresenter.hpp"

#include <gsl/assert>

namespace app::meditation
{
    IntervalChimePresenter ::IntervalChimePresenter(app::ApplicationCommon *app)
        : app{app}, model{std::make_shared<IntervalChimeModel>()}
    {
        model->createData();
    }

    void IntervalChimePresenter::activate(MeditationItem &item)
    {
        model->setData(item);
        model->setOnIntervalChanged([this]() { updateDisplay(); });
        updateDisplay();
    }

    void IntervalChimePresenter::request(MeditationItem &item)
    {
        MeditationItem *p = model->getData();
        if (p != nullptr) {
            item.copyFrom(p);
        }
    }

    void IntervalChimePresenter::increase()
    {
        model->nextInterval();
    }

    void IntervalChimePresenter::decrease()
    {
        model->previousInterval();
    }

    auto IntervalChimePresenter::getIntervalString() -> std::string
    {
        return model->getIntervalString();
    }

    void IntervalChimePresenter::updateDisplay()
    {
        getView()->updateDisplay();
    }
} // namespace app::meditation
