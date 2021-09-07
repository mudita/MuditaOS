
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/FrontlightModel.hpp"
#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace app::bell_settings
{
    class FrontlightWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<FrontlightWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                             = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<FrontlightModel> = 0;
            virtual auto saveData() -> void                                           = 0;
            virtual auto loadData() -> void                                           = 0;
            virtual auto createData() -> void                                         = 0;
        };
    };

    class FrontlightWindowPresenter : public FrontlightWindowContract::Presenter
    {
      public:
        explicit FrontlightWindowPresenter(std::shared_ptr<FrontlightModel> pagesProvider);

        auto saveData() -> void;
        auto loadData() -> void;
        auto createData() -> void;
        auto getPagesProvider() const -> std::shared_ptr<FrontlightModel> override;

      private:
        std::shared_ptr<FrontlightModel> pagesProvider;
    };
} // namespace app::bell_settings
