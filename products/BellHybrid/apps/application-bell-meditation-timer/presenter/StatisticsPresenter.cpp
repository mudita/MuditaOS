// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsPresenter.hpp"

namespace app::meditation
{
    StatisticsPresenter::StatisticsPresenter()
    {}
    void StatisticsPresenter::eraseProviderData()
    {}
    void StatisticsPresenter::loadData()
    {}
    void StatisticsPresenter::saveData()
    {}
    auto StatisticsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return std::shared_ptr<gui::ListItemProvider>();
    }
    void StatisticsPresenter::handleEnter()
    {}

    void StatisticsPresenter::exitWithoutSave()
    {}
} // namespace app::meditation
