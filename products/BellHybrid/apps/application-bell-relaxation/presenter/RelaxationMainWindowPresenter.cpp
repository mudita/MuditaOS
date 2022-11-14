// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationMainWindowPresenter.hpp"
#include <apps-common/models/SongsRepository.hpp>
#include <purefs/filesystem_paths.hpp>

namespace
{
    constexpr auto soundsRepoOffset = 0;
    constexpr auto soundsRepoLimit  = 100;
} // namespace

namespace app::relaxation
{
    RelaxationMainWindowPresenter::RelaxationMainWindowPresenter(
        std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository)
        : soundsRepository{std::move(soundsRepository)}
    {}
    void RelaxationMainWindowPresenter::loadAudioRecords()
    {
        soundsRepository->getMusicFilesList(
            soundsRepoOffset,
            soundsRepoLimit,
            [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                   unsigned int repoRecordsCount) {
                getView()->setSoundsList(records);
                return true;
            });
    }
} // namespace app::relaxation
