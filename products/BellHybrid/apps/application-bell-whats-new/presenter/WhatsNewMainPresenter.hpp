// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <string>

namespace settings
{
    class Settings;
}

namespace app::whatsnew
{
    class WhatsNewMainContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<WhatsNewMainContract::View>
        {
          public:
            virtual ~Presenter()                       = default;
            virtual auto setCurrentOsVersion() -> void = 0;
        };
    };

    class WhatsNewMainPresenter : public WhatsNewMainContract::Presenter
    {
      public:
        explicit WhatsNewMainPresenter(settings::Settings *settings);
        auto setCurrentOsVersion() -> void override;

      private:
        settings::Settings *settings{nullptr};
    };
} // namespace app::whatsnew
