// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RelaxationSongsRepository.hpp"
#include <apps-common/models/SongsModelInterface.hpp>
#include <gui/widgets/ListItemProvider.hpp>

namespace app::relaxation
{

    class RelaxationSongsProvider : public app::DatabaseModel<db::multimedia_files::MultimediaFilesRecord>,
                                    public gui::ListItemProvider
    {
      public:
        using OnActivateCallback =
            std::function<bool(const db::multimedia_files::MultimediaFilesRecord &selectedSound)>;
        explicit RelaxationSongsProvider(ApplicationCommon *application);
        virtual void createData(OnActivateCallback activateCallback) = 0;
    };

    class RelaxationSongsModel : public RelaxationSongsProvider
    {
      private:
        ApplicationCommon *application;
        std::unique_ptr<RelaxationSongsRepository> songsRepository;
        OnActivateCallback activateCallback{nullptr};

        bool onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned int repoRecordsCount);
        [[nodiscard]] bool updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records) override;

      public:
        explicit RelaxationSongsModel(ApplicationCommon *application,
                                      std::unique_ptr<RelaxationSongsRepository> soundsRepository);

        unsigned int requestRecordsCount() override;

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;

        gui::ListItem *getItem(gui::Order order) override;

        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

        void createData(OnActivateCallback activateCallback) override;
    };
} // namespace app::relaxation
