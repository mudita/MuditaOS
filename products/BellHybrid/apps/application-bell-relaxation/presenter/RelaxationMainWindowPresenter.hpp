// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <vector>

namespace app::music
{
    class AbstractSongsRepository;
}
namespace app::relaxation
{
    class RelaxationMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;

            virtual void setSoundsList(std::vector<db::multimedia_files::MultimediaFilesRecord> songs) = 0;
        };

        class Presenter : public BasePresenter<RelaxationMainWindowContract::View>
        {
          public:
            virtual void loadAudioRecords() = 0;
        };
    };

    class RelaxationMainWindowPresenter : public RelaxationMainWindowContract::Presenter
    {
        std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository;
        void loadAudioRecords() override;

      public:
        explicit RelaxationMainWindowPresenter(std::unique_ptr<app::music::AbstractSongsRepository> soundsRepository);
    };

} // namespace app::relaxation
