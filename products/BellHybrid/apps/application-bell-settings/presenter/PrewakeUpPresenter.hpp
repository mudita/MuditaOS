// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PrewakeUpModel.hpp"

#include <apps-common/BasePresenter.hpp>

namespace app::bell_settings
{
    class PrewakeUpWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<PrewakeUpWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                                   = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual auto clearData() -> void                                                = 0;
            virtual auto saveData() -> void                                                 = 0;
            virtual auto loadData() -> void                                                 = 0;
            virtual auto createData() -> void                                               = 0;
        };
    };

    class PrewakeUpWindowPresenter : public PrewakeUpWindowContract::Presenter
    {
      public:
        explicit PrewakeUpWindowPresenter(std::shared_ptr<PrewakeUpModel> pagesProvider);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto clearData() -> void override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto createData() -> void override;

      private:
        std::shared_ptr<PrewakeUpModel> pagesProvider;
    };
} // namespace app::bell_settings
