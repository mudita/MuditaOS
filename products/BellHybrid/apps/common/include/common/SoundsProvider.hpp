// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DatabaseModel.hpp"
#include "module-gui/gui/widgets/ListItemProvider.hpp"
#include "module-db/Interface/MultimediaFilesRecord.hpp"

namespace app
{
    class ApplicationCommon;
}
namespace app::music
{
    class AbstractSongsRepository;
}
namespace app::bgSounds
{
    class SoundsProvider : public app::DatabaseModel<db::multimedia_files::MultimediaFilesRecord>,
                           public gui::ListItemProvider
    {
      public:
        using ItemBuilder = std::function<gui::ListItem *(const db::multimedia_files::MultimediaFilesRecord &)>;
        using OnSoundItemActivatedCallback =
            std::function<void(const db::multimedia_files::MultimediaFilesRecord &sound)>;

        SoundsProvider(app::ApplicationCommon *app,
                       ItemBuilder &&itemBuilder,
                       std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository,
                       std::uint32_t itemSize);

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
        auto getItem(gui::Order order) -> gui::ListItem * override;
        void requestRecords(uint32_t offset, uint32_t limit) override;

        void setOnSoundItemActivatedCallback(OnSoundItemActivatedCallback &&callback);

      private:
        app::ApplicationCommon *app{nullptr};
        std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository;
        ItemBuilder itemBuilder;
        std::uint32_t itemSize;

        OnSoundItemActivatedCallback onSoundItemActivatedCallback;

        [[nodiscard]] bool updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records) override;

        bool onAudioListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  std::uint32_t repoRecordsCount);
    };
} // namespace app::bgSounds
