// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBedtimeWindowPresenter.hpp"
#include "models/BedtimeListItemProvider.hpp"

namespace app::bell_bedtime
{
    BellBedtimeWindowPresenter::BellBedtimeWindowPresenter(std::shared_ptr<BedtimeListItemProvider> provider)
        : provider{provider}
    {
        provider->onExit = [this](bool showSuccessWindow) { getView()->exit(showSuccessWindow); };
    }

    void BellBedtimeWindowPresenter::saveData()
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    void BellBedtimeWindowPresenter::loadData()
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto BellBedtimeWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }
} // namespace app::bell_bedtime
