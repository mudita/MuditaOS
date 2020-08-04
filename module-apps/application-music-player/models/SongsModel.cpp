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

void SongsModel::createData(std::vector<std::string> songsList)
{
    for (auto song : songsList) {

        auto fileTags = AudioServiceAPI::GetFileTags(application, song);
        auto item     = fileTags ? new gui::SongItem(fileTags->title,
                                                 fileTags->artist,
                                                 utils::time::Duration(fileTags->total_duration_s).str())
                             : new gui::SongItem(song, "", utils::time::Duration(0).str());

        item->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("activatedCallback");

            AudioServiceAPI::PlaybackStart(application, song);

            return false;
        };

        internalData.push_back(item);
    }

    for (auto item : internalData) {
        item->deleteByList = false;
    }

    requestRecords(0, internalData.size());
}

void SongsModel::clearData()
{
    list->clear();

    requestRecords(0, internalData.size());
}
