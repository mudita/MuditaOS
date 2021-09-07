// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpPresenter.hpp"

namespace app::bell_settings
{
    PrewakeUpWindowPresenter::PrewakeUpWindowPresenter(std::shared_ptr<PrewakeUpModel> pagesProvider)
        : pagesProvider(std::move(pagesProvider))
    {}

    auto PrewakeUpWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return pagesProvider;
    }

    auto PrewakeUpWindowPresenter::clearData() -> void
    {
        pagesProvider->clearData();
    }

    auto PrewakeUpWindowPresenter::saveData() -> void
    {
        pagesProvider->saveData();
    }

    auto PrewakeUpWindowPresenter::loadData() -> void
    {
        pagesProvider->loadData();
    }

    auto PrewakeUpWindowPresenter::createData() -> void
    {
        pagesProvider->createData();
    }
} // namespace app::bell_settings
