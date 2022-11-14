// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPausedPresenter.hpp"
#include <data/RelaxationCommon.hpp>
#include <ApplicationBellRelaxation.hpp>

namespace app::relaxation
{
    RelaxationPausedPresenter::RelaxationPausedPresenter(std::unique_ptr<AbstractTimeModel> timeModel)
        : timeModel{std::move(timeModel)}
    {}

    void RelaxationPausedPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    void RelaxationPausedPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }
} // namespace app::relaxation
