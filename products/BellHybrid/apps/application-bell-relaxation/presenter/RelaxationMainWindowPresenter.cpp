// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(std::unique_ptr<SongsModel<MusicType>> songsModel)
        : songsModel{std::move(songsModel)}
    {}

    void RelaxationMainWindowPresenter::createData(SongsModel<MusicType>::OnActivateCallback activateCallback)
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

    std::shared_ptr<SongsModel<MusicType>> RelaxationMainWindowPresenter::getSongsModel()
    {
        return songsModel;
    }
} // namespace app::relaxation
