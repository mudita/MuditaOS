// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>

namespace
{
    constexpr auto offset            = 0;
    constexpr auto filesLimitPerPath = 100;
} // namespace

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(
        std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository)
        : soundsRepository{std::move(soundsRepository)}
    {}

    void RelaxationMainWindowPresenter::loadAudioRecords()
    {
        soundsRepository->getMusicFilesListByPaths(
            offset,
            filesLimitPerPath,
            [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                   unsigned int repoRecordsCount) {
                getView()->setSoundsList(records);
                if (repoRecordsCount > filesLimitPerPath) {
                    getView()->handleError();
                }
                return true;
            });
    }
} // namespace app::relaxation
