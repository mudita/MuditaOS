// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <memory>
#include <string>
#include <vector>

namespace app::music
{
    class AbstractSongsRepository;
}
namespace app::bgSounds
{
    class BGSoundsMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;

            virtual void setSoundsList(std::vector<db::multimedia_files::MultimediaFilesRecord> songs) = 0;
        };

        class Presenter : public BasePresenter<BGSoundsMainWindowContract::View>
        {
          public:
            virtual void loadAudioRecords() = 0;
        };
    };

    class BGSoundsMainWindowPresenter : public BGSoundsMainWindowContract::Presenter
    {
        std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository;
        void loadAudioRecords() override;

      public:
        explicit BGSoundsMainWindowPresenter(std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository);
    };

} // namespace app::bgSounds
