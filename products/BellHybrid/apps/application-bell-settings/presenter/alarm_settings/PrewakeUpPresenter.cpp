// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpPresenter.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"

namespace app::bell_settings
{
    PrewakeUpWindowPresenter::PrewakeUpWindowPresenter(std::shared_ptr<PrewakeUpListItemProvider> provider,
                                                       std::unique_ptr<AbstractPrewakeUpSettingsModel> model)
        : provider(std::move(provider)), model(std::move(model))
    {
        this->provider->onExit = [this]() { getView()->exit(); };
    }

    auto PrewakeUpWindowPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto PrewakeUpWindowPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto PrewakeUpWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void PrewakeUpWindowPresenter::eraseProviderData()
    {
        provider->clearData();
    }
} // namespace app::bell_settings
