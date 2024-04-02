// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusSettingsPresenter.hpp"

#include <ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::focus
{
    using namespace gui;
    SettingsPresenter::SettingsPresenter()
    {
        listItemsProvider = std::make_shared<BellListItemProvider>(BellListItemProvider::Items{});

        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::loadData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::saveData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->getValue();
        }
    }

    auto SettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return listItemsProvider;
    }

    void SettingsPresenter::eraseProviderData()
    {
        listItemsProvider->clearData();
    }

    void SettingsPresenter::exitWithSave()
    {
        saveData();
        eraseProviderData();
    }

    void SettingsPresenter::exitWithoutSave()
    {
        eraseProviderData();
    }
} // namespace app::focus
