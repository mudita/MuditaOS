// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/models/SongsModel.hpp>
#include <common/widgets/LabelOption.hpp>

namespace app
{
    SongsProvider::SongsProvider(ApplicationCommon *app) : DatabaseModel(app)
    {}

    SongsModel::SongsModel(ApplicationCommon *application,
                           std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                           const LabelsWithPaths &pathPrefixes)
        : SongsProvider(application),
          application(application), songsRepository{std::move(soundsRepository)}, pathPrefixes{pathPrefixes}
    {}

    void SongsModel::createData(OnActivateCallback callback)
    {
        activateCallback = callback;

        songsRepository->init();
    }

    void SongsModel::updateRecordsCount()
    {
        songsRepository->updateFilesCount();
    }

    unsigned int SongsModel::requestRecordsCount()
    {
        return songsRepository->getFilesCount();
    }

    unsigned int SongsModel::getMinimalItemSpaceRequired() const
    {
        return style::bell_options::h + 2 * style::bell_options::option_margin;
    }

    bool SongsModel::nextRecordExist(gui::Order order)
    {
        const auto getOppositeOrder = [order]() {
            return order == gui::Order::Next ? gui::Order::Previous : gui::Order::Next;
        };

        auto exist = getRecord(order) != nullptr;
        getRecord(getOppositeOrder());
        return exist;
    }

    gui::ListItem *SongsModel::getItem(gui::Order order)
    {
        const auto sound = getRecord(order);
        if (!sound) {
            return nullptr;
        }
        auto item = gui::option::LabelOption{getLabelFromPath(sound->fileInfo.path),
                                             sound->tags.title,
                                             [=]([[maybe_unused]] gui::Item &item) {
                                                 activateCallback(*sound);
                                                 return true;
                                             },
                                             []([[maybe_unused]] gui::Item &item) { return true; },
                                             nullptr};

        return item.build();
    }

    void SongsModel::requestRecords(std::uint32_t offset, std::uint32_t limit)
    {
        songsRepository->getMusicFiles(offset, limit, [this](const auto &records, const auto repoRecordsCount) {
            return onMusicListRetrieved(records, repoRecordsCount);
        });
    }

    bool SongsModel::onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                          unsigned int repoRecordsCount)
    {
        if (list != nullptr && recordsCount != repoRecordsCount) {
            recordsCount = repoRecordsCount;
            list->reSendLastRebuildRequest();
            return false;
        }
        return updateRecords(records);
    }

    bool SongsModel::updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }

    gui::ListLabel SongsModel::getLabelFromPath(const std::string &path)
    {
        for (const auto &[label, pathPrefix] : pathPrefixes) {
            if (path.find(pathPrefix) != std::string::npos) {
                return label;
            }
        }
        return std::nullopt;
    }

} // namespace app
