// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationSongsModel.hpp"
#include "common/options/OptionBellMenu.hpp"

namespace app::relaxation
{

    RelaxationSongsProvider::RelaxationSongsProvider(ApplicationCommon *app) : DatabaseModel(app)
    {}

    RelaxationSongsModel::RelaxationSongsModel(
        ApplicationCommon *application, std::unique_ptr<app::relaxation::RelaxationSongsRepository> soundsRepository)
        : RelaxationSongsProvider(application), application(application), songsRepository{std::move(soundsRepository)}
    {}

    void RelaxationSongsModel::createData(OnActivateCallback callback)
    {
        activateCallback = callback;

        songsRepository->init();
    }

    unsigned int RelaxationSongsModel::requestRecordsCount()
    {
        return songsRepository->getRecordsCount();
    }
    unsigned int RelaxationSongsModel::getMinimalItemSpaceRequired() const
    {
        return style::bell_options::h + 2 * style::bell_options::option_margin;
    }
    gui::ListItem *RelaxationSongsModel::getItem(gui::Order order)
    {
        const auto sound = getRecord(order);
        if (!sound) {
            return nullptr;
        }

        auto item = new gui::option::OptionBellMenu(
            sound->tags.title,
            [=](gui::Item &item) {
                activateCallback(*sound);
                return true;
            },
            [=](gui::Item &item) { return true; },
            nullptr);

        return item->build();
    }
    void RelaxationSongsModel::requestRecords(std::uint32_t offset, std::uint32_t limit)
    {
        songsRepository->getMusicFiles(
            offset,
            limit,
            [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                   unsigned int repoRecordsCount) { return onMusicListRetrieved(records, repoRecordsCount); });
    }
    bool RelaxationSongsModel::onMusicListRetrieved(
        const std::vector<db::multimedia_files::MultimediaFilesRecord> &records, unsigned int repoRecordsCount)
    {
        if (list != nullptr && recordsCount != repoRecordsCount) {
            recordsCount = repoRecordsCount;
            list->reSendLastRebuildRequest();
            return false;
        }
        return updateRecords(records);
    }
    bool RelaxationSongsModel::updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }
} // namespace app::relaxation
