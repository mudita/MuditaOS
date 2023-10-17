// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-settings/models/DateTimeUnitsModel.hpp>

#include <apps-common/BasePresenter.hpp>

namespace app::bell_settings
{
    class AbstractTemperatureUnitModel;
    class AbstractLayoutModel;

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
        explicit TimeUnitsWindowPresenter(app::ApplicationCommon *app,
                                          std::shared_ptr<DateTimeUnitsModel> pagesProvider,
                                          std::unique_ptr<AbstractTemperatureUnitModel> temperatureUnitModel,
                                          std::unique_ptr<AbstractLayoutModel> layoutModel);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto clearData() -> void override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto createData() -> void override;

      private:
        app::ApplicationCommon *app{};
        std::shared_ptr<DateTimeUnitsModel> pagesProvider;
        std::unique_ptr<AbstractTemperatureUnitModel> temperatureUnitModel;
        std::unique_ptr<AbstractLayoutModel> layoutModel;
    };
} // namespace app::bell_settings
