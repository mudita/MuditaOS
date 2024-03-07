// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/models/SongsModelInterface.hpp>
#include <gui/widgets/ListItemProvider.hpp>
#include <common/options/OptionBellMenu.hpp>
#include <common/widgets/LabelOption.hpp>
#include <common/SoundsRepository.hpp>
#include <common/widgets/LabelListItem.hpp>

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

    template <typename T>
    class SongsModel : public SongsProvider
    {
      private:
        ApplicationCommon *application;
        std::unique_ptr<AbstractSoundsRepository> songsRepository;
        std::map<T, std::string> pathPrefixes;
        OnActivateCallback activateCallback{nullptr};

        bool onMusicListRetrieved(const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                                  unsigned int repoRecordsCount)
        {
            if (list != nullptr && recordsCount != repoRecordsCount) {
                recordsCount = repoRecordsCount;
                list->reSendLastRebuildRequest();
                return false;
            }
            return updateRecords(records);
        }
        bool updateRecords(std::vector<db::multimedia_files::MultimediaFilesRecord> records)
        {
            DatabaseModel::updateRecords(std::move(records));
            list->onProviderDataUpdate();
            return true;
        }
        std::optional<T> getTypeFromPath(const std::string &path)
        {
            for (const auto &[type, pathPrefix] : pathPrefixes) {
                if (path.find(pathPrefix) != std::string::npos) {
                    return type;
                }
            }
            return std::nullopt;
        }

      public:
        virtual ~SongsModel() = default;

        SongsModel(ApplicationCommon *application,
                   std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                   const std::map<T, std::string> &pathPrefixes)
            : SongsProvider(application),
              application(application), songsRepository{std::move(soundsRepository)}, pathPrefixes{pathPrefixes}
        {}

        unsigned int requestRecordsCount()
        {
            return songsRepository->getFilesCount();
        }
        unsigned int getMinimalItemSpaceRequired() const
        {
            return style::bell_options::h + 2 * style::bell_options::option_margin;
        }
        gui::ListItem *getItem(gui::Order order)
        {
            const auto sound = getRecord(order);
            if (!sound) {
                return nullptr;
            }
            auto item = gui::option::LabelOption<T>{getTypeFromPath(sound->fileInfo.path),
                                                    sound->tags.title,
                                                    [=]([[maybe_unused]] gui::Item &item) {
                                                        activateCallback(*sound);
                                                        return true;
                                                    },
                                                    []([[maybe_unused]] gui::Item &item) { return true; },
                                                    nullptr};

            return item.build();
        }
        void requestRecords(std::uint32_t offset, std::uint32_t limit)
        {
            songsRepository->getMusicFiles(
                offset,
                limit,
                [this](const std::vector<db::multimedia_files::MultimediaFilesRecord> &records,
                       unsigned int repoRecordsCount) { return onMusicListRetrieved(records, repoRecordsCount); });
        }
        void createData(OnActivateCallback callback)
        {
            activateCallback = callback;

            songsRepository->init();
        }
        void updateRecordsCount()
        {
            songsRepository->updateFilesCount();
        }
        bool nextRecordExist(gui::Order order)
        {
            const auto getOppositeOrder = [order]() {
                return order == gui::Order::Next ? gui::Order::Previous : gui::Order::Next;
            };

            auto exist = getRecord(order) != nullptr;
            getRecord(getOppositeOrder());
            return exist;
        }
    };
} // namespace app
