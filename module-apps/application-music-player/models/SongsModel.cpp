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

    SongsModel::SongsModel(std::shared_ptr<AbstractSongsRepository> songsRepository)
        : songsRepository{std::move(songsRepository)}
    {}

    auto SongsModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto SongsModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return musicPlayerStyle::songItem::h + style::margins::small * 2;
    }

    void SongsModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto SongsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void SongsModel::createData(OnShortReleaseCallback shortReleaseCallback,
                                OnLongPressCallback longPressCallback,
                                OnSetBottomBarTemporaryCallback bottomBarTemporaryMode,
                                OnRestoreBottomBarTemporaryCallback bottomBarRestoreFromTemporaryMode)
    {
        songsRepository->scanMusicFilesList();
        auto songsList = songsRepository->getMusicFilesList();
        for (const auto &song : songsList) {
            auto item = new gui::SongItem(song.artist,
                                          song.title,
                                          utils::time::Duration(song.total_duration_s).str(),
                                          bottomBarTemporaryMode,
                                          bottomBarRestoreFromTemporaryMode);

            item->activatedCallback = [=](gui::Item &) {
                shortReleaseCallback(song.filePath);
                return true;
            };

            item->inputCallback = [longPressCallback](gui::Item &, const gui::InputEvent &event) {
                if (event.isLongRelease(gui::KeyCode::KEY_ENTER)) {
                    longPressCallback();
                    return true;
                }
                return false;
            };

            internalData.push_back(item);
        }

        for (auto &item : internalData) {
            item->deleteByList = false;
        }
    }

    bool SongsModel::isSongPlaying() const noexcept
    {
        return songContext.currentSongState == SongState::Playing;
    }

    void SongsModel::setCurrentSongState(SongState songState) noexcept
    {
        songContext.currentSongState = songState;
        updateCurrentItemState();
    }

    std::optional<audio::Token> SongsModel::getCurrentFileToken() const noexcept
    {
        return songContext.currentFileToken;
    }

    size_t SongsModel::getCurrentIndex() const
    {
        auto index = songsRepository->getFileIndex(songContext.filePath);
        return index == std::numeric_limits<size_t>::max() ? 0 : index;
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
        using namespace gui;
        clearCurrentItemState();

        songContext = context;

        updateCurrentItemState();
    }

    void SongsModel::clearCurrentSongContext()
    {
        clearCurrentItemState();
        songContext.clear();
    }

    void SongsModel::clearCurrentItemState()
    {
        using namespace gui;
        const auto songIndex = getCurrentIndex();
        if (songIndex < internalData.size()) {
            internalData[songIndex]->setState(SongItem::ItemState::None);
        }
    }

    void SongsModel::updateCurrentItemState()
    {
        using namespace gui;
        const auto songIndex = getCurrentIndex();
        if (songIndex >= internalData.size()) {
            return;
        }

        if (songContext.isPlaying()) {
            internalData[songIndex]->setState(SongItem::ItemState::Playing);
        }
        else if (songContext.isPaused()) {
            internalData[songIndex]->setState(SongItem::ItemState::Paused);
        }
        else {
            internalData[songIndex]->setState(SongItem::ItemState::None);
        }
    }

    void SongsModel::clearData()
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::music_player
