// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class AudioErrorContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public app::BasePresenter<AudioErrorContract::View>
        {
          public:
            virtual void activate() = 0;
        };
    };

    class AudioErrorPresenter : public AudioErrorContract::Presenter
    {
        app::ApplicationCommon *app{};
        void activate() override;

      public:
        explicit AudioErrorPresenter(app::ApplicationCommon *app);
    };
} // namespace gui
