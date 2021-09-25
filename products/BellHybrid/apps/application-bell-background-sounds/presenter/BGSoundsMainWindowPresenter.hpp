// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <memory>
#include <string>
#include <vector>

namespace app::bgSounds
{
    class BGSoundsMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;

            virtual void setSoundsList(std::vector<tags::fetcher::Tags> soundsTags) = 0;
        };

        class Presenter : public BasePresenter<BGSoundsMainWindowContract::View>
        {
          public:
            virtual void loadAudioRecords() = 0;
        };
    };

    class AbstractSoundsRepository;
    class BGSoundsMainWindowPresenter : public BGSoundsMainWindowContract::Presenter
    {
        std::shared_ptr<AbstractSoundsRepository> soundsRepository;
        void loadAudioRecords() override;

      public:
        explicit BGSoundsMainWindowPresenter(std::shared_ptr<AbstractSoundsRepository> soundsRepository);
    };

} // namespace app::bgSounds
