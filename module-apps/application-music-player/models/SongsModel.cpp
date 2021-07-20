// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsModel.hpp"
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
        return musicPlayerStyle::songItem::h;
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

    void SongsModel::createData(std::function<bool(const std::string &fileName)> func)
    {
        auto songsList = songsRepository->getMusicFilesList();
        for (const auto &song : songsList) {
            auto item = new gui::SongItem(song.artist, song.title, utils::time::Duration(song.total_duration_s).str());

            item->activatedCallback = [=](gui::Item &) {
                func(song.filePath);
                return true;
            };

            internalData.push_back(item);
        }

        for (auto &item : internalData) {
            item->deleteByList = false;
        }

        list->rebuildList();
    }

    void SongsModel::clearData()
    {
        list->reset();

        list->rebuildList();
    }

    bool SongsModel::isSongPlaying() const noexcept
    {
        return currentSongState == SongState::Playing;
    }

    void SongsModel::setCurrentSongState(SongState songState) noexcept
    {
        currentSongState = songState;
    }

    std::optional<audio::Token> SongsModel::getCurrentFileToken() const noexcept
    {
        return currentFileToken;
    }

    void SongsModel::setCurrentFileToken(std::optional<audio::Token> token) noexcept
    {
        currentFileToken = token;
    }
} // namespace app::music_player
