// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/SoundsRepository.hpp>
#include <common/widgets/LabelListItem.hpp>
#include <apps-common/models/SongsModelInterface.hpp>
#include <gui/widgets/ListItemProvider.hpp>

namespace app
{
    using LabelsWithPaths = std::map<std::string, std::string>;

    class SongsProvider : public app::DatabaseModel<db::multimedia_files::MultimediaFilesRecord>,
                          public gui::ListItemProvider
    {
      public:
        using OnActivateCallback =
            std::function<bool(const db::multimedia_files::MultimediaFilesRecord &selectedSound)>;
        using OnFocusAcquireCallback =
            std::function<bool(const db::multimedia_files::MultimediaFilesRecord &focusedSound)>;

        explicit SongsProvider(ApplicationCommon *application);
        virtual ~SongsProvider() = default;

        virtual auto createData(OnActivateCallback activateCallback, OnFocusAcquireCallback focusChangeCallback)
            -> void = 0;
    };

    class SongsModel : public SongsProvider
    {
      public:
        SongsModel(ApplicationCommon *application,
                   std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                   const LabelsWithPaths &pathPrefixes = {});
        virtual ~SongsModel() = default;

        auto requestRecordsCount() -> unsigned override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned override;

        auto getItem(gui::Order order) -> gui::ListItem * override;

        auto requestRecords(std::uint32_t offset, std::uint32_t limit) -> void override;

        auto createData(OnActivateCallback activateCallback        = nullptr,
                        OnFocusAcquireCallback focusChangeCallback = nullptr) -> void override;
        auto updateRecordsCount() -> void;
        auto nextRecordExists(gui::Order order) -> bool;

        auto setCurrentlyChosenRecordPath(const std::string &path) -> void;
        [[nodiscard]] auto getCurrentlyChosenRecordPath() const -> std::string;

      private:
        auto onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned repoRecordsCount) -> bool;
        [[nodiscard]] auto updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
            -> bool override;
        auto getLabelFromPath(const std::string &path) -> gui::ListLabel;

        ApplicationCommon *application{nullptr};
        std::unique_ptr<AbstractSoundsRepository> songsRepository;
        LabelsWithPaths pathPrefixes;
        OnActivateCallback activateCallback{nullptr};
        OnFocusAcquireCallback focusAcquireCallback{nullptr};
        std::string currentlyChosenRecordPath{};
    };
} // namespace app
