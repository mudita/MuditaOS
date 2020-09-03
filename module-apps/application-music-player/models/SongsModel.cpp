#include "SongsModel.hpp"
#include "application-music-player/widgets/SongItem.hpp"

#include <ListView.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>
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

void SongsModel::createData(std::vector<audio::Tags> songsList, std::function<bool(const std::string &fileName)> func)
{
    for (auto song : songsList) {
        auto item = new gui::SongItem(song.title, song.artist, utils::time::Duration(song.total_duration_s).str());

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
    list->clear();

    list->rebuildList();
}
