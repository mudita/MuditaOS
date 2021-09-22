// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/TemperatureUnitModel.hpp"
#include "presenter/TimeUnitsPresenter.hpp"

namespace app::bell_settings
{
    TimeUnitsWindowPresenter::TimeUnitsWindowPresenter(
        std::shared_ptr<TimeUnitsModel> pagesProvider,
        std::unique_ptr<AbstractTemperatureUnitModel> temperatureUnitModel)
        : pagesProvider(std::move(pagesProvider)), temperatureUnitModel{std::move(temperatureUnitModel)}
    {}

    auto TimeUnitsWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return pagesProvider;
    }

    auto TimeUnitsWindowPresenter::clearData() -> void
    {
        pagesProvider->clearData();
    }

    auto TimeUnitsWindowPresenter::saveData() -> void
    {
        pagesProvider->saveData();
        temperatureUnitModel->set(pagesProvider->getTemperatureUnit());
    }

    auto TimeUnitsWindowPresenter::loadData() -> void
    {
        pagesProvider->loadData();
        pagesProvider->setTemperatureUnit(temperatureUnitModel->get());
    }

    auto TimeUnitsWindowPresenter::createData() -> void
    {
        pagesProvider->createData();
    }
} // namespace app::bell_settings
