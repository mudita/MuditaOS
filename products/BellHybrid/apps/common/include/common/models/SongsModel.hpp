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
        using OnFocusChangeCallback =
            std::function<bool(const db::multimedia_files::MultimediaFilesRecord &focusedSound)>;

        explicit SongsProvider(ApplicationCommon *application);
        virtual ~SongsProvider() = default;

        virtual auto createData(OnActivateCallback activateCallback, OnFocusChangeCallback focusChangeCallback)
            -> void = 0;
    };

    class SongsModel : public SongsProvider
    {
      public:
        SongsModel(ApplicationCommon *application,
                   std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                   const LabelsWithPaths &pathPrefixes);
        virtual ~SongsModel() = default;

        unsigned int requestRecordsCount() override;

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;

        gui::ListItem *getItem(gui::Order order) override;

        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

        void createData(OnActivateCallback activateCallback, OnFocusChangeCallback focusChangeCallback) override;
        void updateRecordsCount();
        bool nextRecordExist(gui::Order order);
        unsigned int getRecordIndexForPath(const std::string &path);
        //        void setCurrentRecord(const std::string &path);

      private:
        bool onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned int repoRecordsCount);
        [[nodiscard]] bool updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records) override;
        gui::ListLabel getLabelFromPath(const std::string &path);

        ApplicationCommon *application;
        std::unique_ptr<AbstractSoundsRepository> songsRepository;
        LabelsWithPaths pathPrefixes;
        std::string currentRecordPath;
        OnActivateCallback activateCallback{nullptr};
        OnFocusChangeCallback focusChangeCallback{nullptr};
    };
} // namespace app
