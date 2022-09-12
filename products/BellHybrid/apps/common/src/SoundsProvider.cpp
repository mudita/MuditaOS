// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "products/BellHybrid/apps/common/include/common/SoundsProvider.hpp"
#include "module-gui/gui/widgets/ListViewEngine.hpp"
#include "module-apps/apps-common/models/SongsRepository.hpp"

namespace app::bgSounds
{
    app::bgSounds::SoundsProvider::SoundsProvider(app::ApplicationCommon *app,
                                                  ItemBuilder &&itemBuilder,
                                                  std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository,
                                                  std::uint32_t itemSize)
        : DatabaseModel(app),
          soundsRepository(std::move(soundsRepository)), itemBuilder{std::move(itemBuilder)}, itemSize{itemSize}
    {}

    auto SoundsProvider::requestRecordsCount() -> unsigned int
    {
        return recordsCount;
    }
    auto SoundsProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return itemSize;
    }
    auto SoundsProvider::getItem(gui::Order order) -> gui::ListItem *
    {
        auto sound = getRecord(order);
        if (!sound) {
            return nullptr;
        }

        auto item               = itemBuilder(*sound);
        item->activatedCallback = [this, sound](auto &) {
            if (onSoundItemActivatedCallback) {
                onSoundItemActivatedCallback(*sound);
            }
            return true;
        };

        return item;
    }
    void SoundsProvider::requestRecords(uint32_t offset, uint32_t limit)
    {
        soundsRepository->getMusicFilesList(
            offset,
            limit,
            [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                   const auto repoRecordsCount) { return onAudioListRetrieved(records, repoRecordsCount); });
    }
    bool SoundsProvider::onAudioListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                              const std::uint32_t repoRecordsCount)
    {
        if (list != nullptr && recordsCount != repoRecordsCount) {
            recordsCount = repoRecordsCount;
            list->reSendLastRebuildRequest();
            return false;
        }
        return updateRecords(records);
    }
    bool SoundsProvider::updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }
    void SoundsProvider::setOnSoundItemActivatedCallback(SoundsProvider::OnSoundItemActivatedCallback &&callback)
    {
        onSoundItemActivatedCallback = std::move(callback);
    }
} // namespace app::bgSounds
