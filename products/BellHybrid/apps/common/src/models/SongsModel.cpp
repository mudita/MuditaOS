// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/models/SongsModel.hpp>
#include <common/widgets/LabelOptionWithTick.hpp>

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

    auto SongsModel::requestRecordsCount() -> unsigned
    {
        return songsRepository->getFilesCount();
    }

    auto SongsModel::getMinimalItemSpaceRequired() const -> unsigned
    {
        return style::bell_options::h + 3 * style::bell_options::option_margin;
    }

    auto SongsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        using TickState = gui::option::LabelOptionWithTick::TickState;

        const auto sound = getRecord(order);
        if (sound == nullptr) {
            return nullptr;
        }

        const auto &filePath = sound->fileInfo.path;
        const auto tickState = (filePath == currentlyChosenRecordPath) ? TickState::Show : TickState::Hide;
        auto item            = gui::option::LabelOptionWithTick{getLabelFromPath(filePath),
                                                     sound->tags.title,
                                                     tickState,
                                                     [this, sound]([[maybe_unused]] gui::Item &item) {
                                                         if (activateCallback) {
                                                             return activateCallback(*sound);
                                                         }
                                                         return true;
                                                     },
                                                     [this, sound](gui::Item &item) {
                                                         if (focusAcquireCallback && item.focus) {
                                                             return focusAcquireCallback(*sound);
                                                         }
                                                         return true;
                                                     },
                                                     nullptr};
        return item.build();
    }

    auto SongsModel::requestRecords(std::uint32_t offset, std::uint32_t limit) -> void
    {
        songsRepository->getMusicFiles(offset, limit, [this](const auto &records, const auto repoRecordsCount) {
            return onMusicListRetrieved(records, repoRecordsCount);
        });
    }

    auto SongsModel::createData(OnActivateCallback onActivate,
                                OnFocusAcquireCallback onFocusAcquire,
                                OnOffsetUpdateCallback offsetUpdateCallback,
                                const std::string &chosenRecordPath) -> void
    {
        currentlyChosenRecordPath = chosenRecordPath;
        activateCallback          = std::move(onActivate);
        focusAcquireCallback      = std::move(onFocusAcquire);
        songsRepository->init(currentlyChosenRecordPath, std::move(offsetUpdateCallback));
    }

    auto SongsModel::updateRecordsCount() -> void
    {
        songsRepository->updateFilesCount();
    }

    auto SongsModel::nextRecordExists(gui::Order order) -> bool
    {
        auto exists = getRecord(order) != nullptr;
        getRecord(~order);
        return exists;
    }

    auto SongsModel::onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                          unsigned int repoRecordsCount) -> bool
    {
        if (list != nullptr && recordsCount != repoRecordsCount) {
            recordsCount = repoRecordsCount;
            list->reSendLastRebuildRequest();
            return false;
        }
        return updateRecords(records);
    }

    auto SongsModel::updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records) -> bool
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }

    auto SongsModel::getLabelFromPath(const std::string &path) -> std::string
    {
        for (const auto &[label, pathPrefix] : pathPrefixes) {
            if (path.find(pathPrefix) != std::string::npos) {
                return label;
            }
        }
        return {};
    }

    auto SongsModel::getLabelsFilesCount() -> std::vector<std::pair<std::string, std::uint32_t>>
    {
        std::vector<std::pair<std::string, std::uint32_t>> labelWithFilesCount;
        for (const auto &[label, pathPrefix] : pathPrefixes) {
            const auto count = songsRepository->getFilesCountFromPath(pathPrefix);
            labelWithFilesCount.push_back({label, count});
        }
        return labelWithFilesCount;
    }

    auto SongsModel::updateCurrentlyChosenRecordPath(const std::string &path) -> void
    {
        currentlyChosenRecordPath = path;
    }

    auto SongsModel::getCurrentlyChosenRecordPath() const -> std::string
    {
        return currentlyChosenRecordPath;
    }

    auto SongsModel::fileExists(const std::string &path) -> bool
    {
        std::error_code ec;
        return std::filesystem::exists(path, ec);
    }
} // namespace app
