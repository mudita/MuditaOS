// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace app::bell_settings
{
    BedTimeSettingsPresenter::BedTimeSettingsPresenter(std::unique_ptr<SongsModel> songsModel)
        : songsModel{std::move(songsModel)}
    {}

    void BedTimeSettingsPresenter::createData(SongsModel::OnActivateCallback activateCallback)
    {
        songsModel->createData(activateCallback);
        updateViewState();
    }
    void BedTimeSettingsPresenter::updateViewState()
    {
        auto view = getView();
        if (view != nullptr) {
            view->updateViewState();
        }
    }
    void BedTimeSettingsPresenter::updateRecordsCount()
    {
        songsModel->updateRecordsCount();
    }
    std::shared_ptr<SongsModel> BedTimeSettingsPresenter::getSongsModel()
    {
        return songsModel;
    }
} // namespace app::bell_settings
