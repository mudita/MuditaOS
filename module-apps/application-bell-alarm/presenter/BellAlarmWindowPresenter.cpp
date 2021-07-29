// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindowPresenter.hpp"

namespace app::bell_alarm
{
    BellAlarmWindowPresenter::BellAlarmWindowPresenter(std::shared_ptr<BellAlarmWindowModel> pagesProvider)
        : pagesProvider(std::move(pagesProvider))
    {}

    auto BellAlarmWindowPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return pagesProvider;
    }

    auto BellAlarmWindowPresenter::clearData() -> void
    {
        pagesProvider->clearData();
    }

    auto BellAlarmWindowPresenter::saveData() -> void
    {
        pagesProvider->saveData();
    }

    auto BellAlarmWindowPresenter::loadData() -> void
    {
        pagesProvider->loadData();
    }

    auto BellAlarmWindowPresenter::createData() -> void
    {
        pagesProvider->createData();
    }
} // namespace app::bell_alarm
