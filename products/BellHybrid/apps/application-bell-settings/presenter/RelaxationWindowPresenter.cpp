// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "RelaxationWindowPresenter.hpp"
#include "models/RelaxationListItemProvider.hpp"

namespace app::bell_settings
{
    RelaxationWindowPresenter::RelaxationWindowPresenter(std::shared_ptr<RelaxationListItemProvider> provider)
        : provider{provider}
    {
        provider->onExit = [this]() { getView()->exit(); };
    }

    void RelaxationWindowPresenter::saveData()
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    void RelaxationWindowPresenter::loadData()
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto RelaxationWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }
} // namespace app::bell_settings
