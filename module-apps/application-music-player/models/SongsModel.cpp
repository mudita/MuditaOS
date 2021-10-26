// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsModel.hpp"
#include "Style.hpp"
#include "application-music-player/widgets/SongItem.hpp"

#include <ListView.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/time_conversion.hpp>

namespace app::music_player
{

    SongsListItemProvider::SongsListItemProvider(ApplicationCommon *app) : DatabaseModel(app)
    {}

    SongsModelInterface::SongsModelInterface(ApplicationCommon *app) : SongsListItemProvider(app)
    {}

    SongsModel::SongsModel(app::ApplicationCommon *app, std::shared_ptr<AbstractSongsRepository> songsRepository)
        : SongsModelInterface(app), songsRepository{std::move(songsRepository)}
    {}

    auto SongsModel::requestRecordsCount() -> unsigned int
    {
        return recordsCount;
    }

    auto SongsModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return musicPlayerStyle::songItem::h + style::margins::small * 2;
    }

    void SongsModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        songsRepository->getMusicFilesList(
            offset,
            limit,
            [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                   unsigned int repoRecordsCount) { return onMusicListRetrieved(records, repoRecordsCount); });
    }

    auto SongsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        std::shared_ptr<db::multimedia_files::MultimediaFilesRecord> song = getRecord(order);
        if (!song) {
            return nullptr;
        }

        auto item = new gui::SongItem(song->tags.album.artist,
                                      song->tags.title,
                                      utils::time::Duration(song->audioProperties.songLength).str(),
                                      navBarTemporaryMode,
                                      navBarRestoreFromTemporaryMode);

        if (songContext.filePath == song->fileInfo.path) {
            item->setState(songContext.isPlaying() ? gui::SongItem::ItemState::Playing
                                                   : (songContext.isPaused() ? gui::SongItem::ItemState::Paused
                                                                             : gui::SongItem::ItemState::None));
        }
        else {
            item->setState(gui::SongItem::ItemState::None);
        }

        item->activatedCallback = [this, song](gui::Item &) {
            if (shortReleaseCallback != nullptr) {
                activatedRecord = *song;
                shortReleaseCallback(song->fileInfo.path);
                return true;
            }
            return false;
        };

        item->inputCallback = [=](gui::Item &, const gui::InputEvent &event) {
            if (event.isLongRelease(gui::KeyCode::KEY_ENTER)) {
                if (longPressCallback != nullptr) {
                    longPressCallback();
                    return true;
                }
            }
            return false;
        };

        return item;
    }

    void SongsModel::createData(OnShortReleaseCallback shortReleaseCallback,
                                OnLongPressCallback longPressCallback,
                                OnSetNavBarTemporaryCallback navBarTemporaryMode,
                                OnRestoreNavBarTemporaryCallback navBarRestoreFromTemporaryMode)
    {
        this->shortReleaseCallback              = shortReleaseCallback;
        this->longPressCallback                 = longPressCallback;
        this->navBarTemporaryMode               = navBarTemporaryMode;
        this->navBarRestoreFromTemporaryMode    = navBarRestoreFromTemporaryMode;

        songsRepository->initCache();
    }

    bool SongsModel::isSongPlaying() const noexcept
    {
        return songContext.currentSongState == SongState::Playing;
    }

    void SongsModel::setCurrentSongState(SongState songState) noexcept
    {
        songContext.currentSongState = songState;
    }

    std::optional<audio::Token> SongsModel::getCurrentFileToken() const noexcept
    {
        return songContext.currentFileToken;
    }

    std::optional<db::multimedia_files::MultimediaFilesRecord> SongsModel::getActivatedRecord() const noexcept
    {
        return activatedRecord;
    }

    std::string SongsModel::getNextFilePath(const std::string &filePath) const
    {
        return songsRepository->getNextFilePath(filePath);
    }

    std::string SongsModel::getPreviousFilePath(const std::string &filePath) const
    {
        return songsRepository->getPreviousFilePath(filePath);
    }

    SongContext SongsModel::getCurrentSongContext() const noexcept
    {
        return songContext;
    }

    void SongsModel::setCurrentSongContext(SongContext context)
    {
        songContext = context;
        activatedRecord = songsRepository->getRecord(context.filePath);
    }

    void SongsModel::clearCurrentSongContext()
    {
        songContext.clear();
        activatedRecord = std::nullopt;
    }

    void SongsModel::clearData()
    {
        if (list != nullptr) {
            list->reset();
        }
    }

    [[nodiscard]] bool SongsModel::updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
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

    void SongsModel::updateRepository(const std::string &filePath)
    {
        songsRepository->updateRepository(filePath);
    }

} // namespace app::music_player
