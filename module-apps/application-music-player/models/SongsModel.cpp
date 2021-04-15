// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SongsModel.hpp"
#include "application-music-player/widgets/SongItem.hpp"

#include <ListView.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <time/time_conversion.hpp>

SongsModel::SongsModel(app::Application *app) : application(app)
{}

auto SongsModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto SongsModel::getMinimalItemHeight() const -> unsigned int
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

void SongsModel::createData(const std::vector<audio::Tags> &songsList,
                            std::function<bool(const std::string &fileName)> func)
{
    for (const auto &song : songsList) {
        auto item = new gui::SongItem(song.artist, song.title, utils::time::Duration(song.total_duration_s).str());

        item->activatedCallback = [=](gui::Item &) {
            func(song.filePath);
            return true;
        };

        internalData.push_back(item);
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList();
}

void SongsModel::clearData()
{
    list->reset();

    list->rebuildList();
}
