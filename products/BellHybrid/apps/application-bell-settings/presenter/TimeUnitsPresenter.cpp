// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/TemperatureUnitModel.hpp"
#include "presenter/TimeUnitsPresenter.hpp"

#include <common/layouts/HomeScreenLayouts.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/layouts/HomeScreenLayoutNames.hpp>

#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>

namespace
{
    constexpr auto classicPrefix  = "Classic";
    constexpr auto verticalPrefix = "Vertical";

} // namespace

namespace app::bell_settings
{
    TimeUnitsWindowPresenter::TimeUnitsWindowPresenter(
        app::ApplicationCommon *app,
        std::shared_ptr<DateTimeUnitsModel> pagesProvider,
        std::unique_ptr<AbstractTemperatureUnitModel> temperatureUnitModel,
        std::unique_ptr<AbstractLayoutModel> layoutModel)
        : app{app}, pagesProvider(std::move(pagesProvider)), temperatureUnitModel{std::move(temperatureUnitModel)},
          layoutModel{std::move(layoutModel)}
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
        auto timeFormat    = pagesProvider->getTimeFormat();
        auto currentLayout = layoutModel->getValue();

        auto isCurrentLayout12h = true;
        if (timeFormat == utils::time::Locale::TimeFormat::FormatTime24H) {
            for (const auto &layout : gui::factory::getLayoutsFormat24h()) {
                if (layout.first == currentLayout) {
                    isCurrentLayout12h = false;
                    break;
                }
            }
            if (isCurrentLayout12h) {
                std::string fallbackLayout;
                if (currentLayout->rfind(classicPrefix, 0) == 0) {
                    fallbackLayout = gui::layout::Classic;
                }
                else if (currentLayout->rfind(verticalPrefix, 0) == 0) {
                    fallbackLayout = gui::layout::VerticalSimple;
                }
                auto layoutChangeRequest = std::make_shared<ChangeHomescreenLayoutMessage>(fallbackLayout);
                app->bus.sendUnicast(layoutChangeRequest, service::name::appmgr);
            }
        }
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
