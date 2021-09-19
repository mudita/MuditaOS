// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
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

            virtual void setSoundsList() = 0;
        };

        class Presenter : public BasePresenter<BGSoundsMainWindowContract::View>
        {
          public:
            virtual void loadAudioRecords() = 0;
        };
    };

    class BGSoundsMainWindowPresenter : public BGSoundsMainWindowContract::Presenter
    {
        void loadAudioRecords() override;

      public:
        explicit BGSoundsMainWindowPresenter();
    };

} // namespace app::bgSounds
