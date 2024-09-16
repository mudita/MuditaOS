// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(std::unique_ptr<SongsModel> songsModel)
        : songsModel{std::move(songsModel)}
    {}

    void RelaxationMainWindowPresenter::createData(SongsModel::OnActivateCallback activateCallback)
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

    std::shared_ptr<SongsModel> RelaxationMainWindowPresenter::getSongsModel()
    {
        return songsModel;
    }
} // namespace app::relaxation
