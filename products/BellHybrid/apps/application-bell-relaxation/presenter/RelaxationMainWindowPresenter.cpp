// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(std::unique_ptr<RelaxationSongsModel> songsModel,
                                                                 AbstractAlarmModel &alarm)
        : songsModel{std::move(songsModel)}, alarmModel{alarm}
    {}

    void RelaxationMainWindowPresenter::createData(RelaxationSongsModel::OnActivateCallback activateCallback)
    {
        songsModel->createData(activateCallback);
        updateViewState();
    }
    void RelaxationMainWindowPresenter::updateViewState()
    {
        auto view = getView();
        if (view != nullptr) {
            view->updateViewState();
        }
    }

    void RelaxationMainWindowPresenter::updateRecordsCount()
    {
        songsModel->updateRecordsCount();
    }

    std::shared_ptr<RelaxationSongsModel> RelaxationMainWindowPresenter::getSongsModel()
    {
        return songsModel;
    }

    bool RelaxationMainWindowPresenter::handleIfPreWakeupIsToTurnOffFirst()
    {
        if (alarmModel.isPreWakeUpActive()) {
            alarmModel.turnOffPreWakeUp();
            return true;
        }
        return false;
    }
} // namespace app::relaxation
