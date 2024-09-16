// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BedsidePresenter.hpp"
#include "BedsideListItemProvider.hpp"

namespace app::bell_settings
{
    BedsidePresenter::BedsidePresenter(std::shared_ptr<BedsideListItemProvider> &&provider)
        : provider{std::move(provider)}
    {}

    auto BedsidePresenter::saveData() -> void
    {
        for (auto item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto BedsidePresenter::loadData() -> void
    {
        for (auto item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto BedsidePresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    auto BedsidePresenter::eraseProviderData() -> void
    {
        provider->clearData();
    }

    auto BedsidePresenter::exitWithoutSave() -> void
    {}
} // namespace app::bell_settings
