// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(std::unique_ptr<RelaxationSongsModel> songsModel)
        : songsModel{std::move(songsModel)}
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

    std::shared_ptr<RelaxationSongsModel> RelaxationMainWindowPresenter::getSongsModel()
    {
        return songsModel;
    }
} // namespace app::relaxation
