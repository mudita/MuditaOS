// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SnoozePresenter.hpp"

#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include <common/models/SnoozeSettingsModel.hpp>

namespace app::bell_settings
{

    SnoozePresenter::SnoozePresenter(std::shared_ptr<SnoozeListItemProvider> provider,
                                     std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel)
        : provider{provider}, snoozeSettingsModel{std::move(snoozeSettingsModel)}
    {
        provider->onExit = [this]() { getView()->exit(); };
    }

    void SnoozePresenter::saveData()
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    void SnoozePresenter::loadData()
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto SnoozePresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }
} // namespace app::bell_settings
