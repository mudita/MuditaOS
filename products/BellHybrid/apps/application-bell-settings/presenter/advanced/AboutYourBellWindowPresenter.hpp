// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace app::bell_settings
{
    class AboutYourBellModel;

    class AboutYourBellWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<AboutYourBellWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                                = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<AboutYourBellModel> = 0;
            virtual auto createData() -> void                                            = 0;
        };
    };

    class AboutYourBellWindowPresenter : public AboutYourBellWindowContract::Presenter
    {
      public:
        explicit AboutYourBellWindowPresenter(std::shared_ptr<AboutYourBellModel> pagesProvider);

        auto createData() -> void override;
        auto getPagesProvider() const -> std::shared_ptr<AboutYourBellModel> override;

      private:
        std::shared_ptr<AboutYourBellModel> pagesProvider;
    };
} // namespace app::bell_settings
