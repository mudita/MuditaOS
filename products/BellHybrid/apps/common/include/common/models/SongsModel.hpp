// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/SoundsRepository.hpp>
#include <common/widgets/LabelListItem.hpp>
#include <apps-common/models/SongsModelInterface.hpp>
#include <gui/widgets/ListItemProvider.hpp>

namespace app
{

    class SongsProvider : public app::DatabaseModel<db::multimedia_files::MultimediaFilesRecord>,
                          public gui::ListItemProvider
    {
      public:
        virtual ~SongsProvider() = default;
        using OnActivateCallback =
            std::function<bool(const db::multimedia_files::MultimediaFilesRecord &selectedSound)>;
        explicit SongsProvider(ApplicationCommon *application);
        virtual void createData(OnActivateCallback activateCallback) = 0;
    };

    class SongsModel : public SongsProvider
    {
      private:
        ApplicationCommon *application;
        std::unique_ptr<AbstractSoundsRepository> songsRepository;
        std::map<int, std::string> pathPrefixes;
        OnActivateCallback activateCallback{nullptr};

        bool onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned int repoRecordsCount);
        [[nodiscard]] bool updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records) override;
        gui::ItemsType getTypeFromPath(const std::string &path);

      public:
        virtual ~SongsModel() = default;

        SongsModel(ApplicationCommon *application,
                   std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                   const std::map<int, std::string> &pathPrefixes);

        unsigned int requestRecordsCount() override;

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;

        gui::ListItem *getItem(gui::Order order) override;

        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

        void createData(OnActivateCallback activateCallback) override;
        void updateRecordsCount();
        bool nextRecordExist(gui::Order order);
    };
} // namespace app
