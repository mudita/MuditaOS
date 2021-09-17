// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSettingsPresenter.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"

namespace app::bell_settings
{
    AlarmSettingsPresenter::AlarmSettingsPresenter(std::shared_ptr<AlarmSettingsListItemProvider> provider,
                                                   std::unique_ptr<AbstractAlarmSettingsModel> model)
        : provider(std::move(provider)), model(std::move(model))
    {
        this->provider->onExit = [this]() { getView()->exit(); };
    }

    auto AlarmSettingsPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto AlarmSettingsPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto AlarmSettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void AlarmSettingsPresenter::eraseProviderData()
    {
        provider->clearData();
    }
} // namespace app::bell_settings
