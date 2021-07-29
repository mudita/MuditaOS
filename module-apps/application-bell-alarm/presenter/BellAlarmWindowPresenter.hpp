// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-alarm/models/BellAlarmWindowModel.hpp>

#include <apps-common/BasePresenter.hpp>

namespace app::bell_alarm
{
    class BellAlarmWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<BellAlarmWindowContract::View>
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

    class BellAlarmWindowPresenter : public BellAlarmWindowContract::Presenter
    {
      public:
        explicit BellAlarmWindowPresenter(std::shared_ptr<BellAlarmWindowModel> pagesProvider);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto clearData() -> void override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto createData() -> void override;

      private:
        std::shared_ptr<BellAlarmWindowModel> pagesProvider;
    };
} // namespace app::bell_alarm
