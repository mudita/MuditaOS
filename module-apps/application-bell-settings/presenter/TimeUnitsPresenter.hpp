// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TimeUnitsModel.hpp"

#include <apps-common/BasePresenter.hpp>

namespace app::bell_settings
{
    class TimeUnitsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<TimeUnitsWindowContract::View>
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

    class TimeUnitsWindowPresenter : public TimeUnitsWindowContract::Presenter
    {
      public:
        explicit TimeUnitsWindowPresenter(std::shared_ptr<TimeUnitsModel> pagesProvider);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto clearData() -> void;
        auto saveData() -> void;
        auto loadData() -> void;
        auto createData() -> void;

      private:
        std::shared_ptr<TimeUnitsModel> pagesProvider;
    };
} // namespace app::bell_settings
